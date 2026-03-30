#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include <immintrin.h>

#define THREADS 4

const float WR = 0.299f;
const float WG = 0.587f;
const float WB = 0.114f;

#pragma pack(push,1)
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB;
#pragma pack(pop)

typedef struct {
    int w;
    int h;
    RGB* buf;
} Img;

typedef struct {
    const Img* in;
    Img* out;
    int r_start;
    int r_end;
    int simd_flag;
} worker_args;

double now_sec()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC,&t);
    return (double)t.tv_sec + (double)t.tv_nsec*1e-9;
}

void ignore_comments(FILE* f)
{
    int c;

    while((c=fgetc(f))!=EOF)
    {
        if(isspace(c)) continue;

        if(c=='#')
        {
            while((c=fgetc(f))!='\n' && c!=EOF);
        }
        else
        {
            ungetc(c,f);
            break;
        }
    }
}

Img read_ppm(const char* name)
{
    FILE* f=fopen(name,"rb");

    Img img={0,0,NULL};

    if(!f) return img;

    char type[3];

    int maxv;

    if(fscanf(f,"%2s",type)!=1 || type[1]!='6')
    {
        fclose(f);
        return img;
    }

    ignore_comments(f);

    fscanf(f,"%d %d",&img.w,&img.h);

    ignore_comments(f);

    fscanf(f,"%d",&maxv);

    fgetc(f);

    size_t total=(size_t)img.w*img.h;

    img.buf=(RGB*)malloc(total*sizeof(RGB));

    if(img.buf)
        fread(img.buf,sizeof(RGB),total,f);

    fclose(f);

    return img;
}

void gray_scalar(const Img* in, Img* out, int rs, int re)
{
    for(int y=rs;y<re;y++)
    {
        for(int x=0;x<in->w;x++)
        {
            int id=y*in->w+x;

            RGB p=in->buf[id];

            uint8_t g=(uint8_t)(WR*p.r+WG*p.g+WB*p.b);

            out->buf[id]=(RGB){g,g,g};
        }
    }
}

void gray_simd(const Img* in, Img* out, int rs, int re)
{
    int w=in->w;

    __m256 vr=_mm256_set1_ps(WR);
    __m256 vg=_mm256_set1_ps(WG);
    __m256 vb=_mm256_set1_ps(WB);

    for(int y=rs;y<re;y++)
    {
        int x=0;

        for(;x<=w-8;x+=8)
        {
            int id=y*w+x;

            float rf[8];
            float gf[8];
            float bf[8];

            for(int i=0;i<8;i++)
            {
                rf[i]=(float)in->buf[id+i].r;
                gf[i]=(float)in->buf[id+i].g;
                bf[i]=(float)in->buf[id+i].b;
            }

            __m256 r=_mm256_loadu_ps(rf);
            __m256 g=_mm256_loadu_ps(gf);
            __m256 b=_mm256_loadu_ps(bf);

            __m256 res=_mm256_add_ps(
                _mm256_mul_ps(r,vr),
                _mm256_add_ps(
                    _mm256_mul_ps(g,vg),
                    _mm256_mul_ps(b,vb)
                )
            );

            float tmp[8];

            _mm256_storeu_ps(tmp,res);

            for(int i=0;i<8;i++)
            {
                uint8_t v=(uint8_t)tmp[i];

                out->buf[id+i]=(RGB){v,v,v};
            }
        }

        for(;x<w;x++)
        {
            int id=y*w+x;

            RGB p=in->buf[id];

            uint8_t g=(uint8_t)(WR*p.r+WG*p.g+WB*p.b);

            out->buf[id]=(RGB){g,g,g};
        }
    }
}

void* worker(void* ptr)
{
    worker_args* a=(worker_args*)ptr;

    if(a->simd_flag)
        gray_simd(a->in,a->out,a->r_start,a->r_end);
    else
        gray_scalar(a->in,a->out,a->r_start,a->r_end);

    return NULL;
}

bool check_gray(const Img* img)
{
    int total=img->w*img->h;

    for(int i=0;i<total;i++)
    {
        if(img->buf[i].r!=img->buf[i].g ||
           img->buf[i].g!=img->buf[i].b)
            return false;
    }

    return true;
}

int main()
{
    Img input=read_ppm("lisalisa.ppm");

    if(!input.buf) return 1;

    Img output={
        input.w,
        input.h,
        malloc(input.w*input.h*sizeof(RGB))
    };

    pthread_t th[THREADS];

    worker_args args[THREADS];

    double t_scalar;
    double t_simd;
    double t_mt;
    double t_mix;

    double start;


    start=now_sec();

    gray_scalar(&input,&output,0,input.h);

    t_scalar=now_sec()-start;


    start=now_sec();

    gray_simd(&input,&output,0,input.h);

    t_simd=now_sec()-start;


    int step=input.h/THREADS;


    start=now_sec();

    for(int i=0;i<THREADS;i++)
    {
        args[i]=(worker_args){
            &input,
            &output,
            i*step,
            (i==THREADS-1)?input.h:(i+1)*step,
            0
        };

        pthread_create(&th[i],NULL,worker,&args[i]);
    }

    for(int i=0;i<THREADS;i++)
        pthread_join(th[i],NULL);

    t_mt=now_sec()-start;


    start=now_sec();

    for(int i=0;i<THREADS;i++)
    {
        args[i].simd_flag=1;

        pthread_create(&th[i],NULL,worker,&args[i]);
    }

    for(int i=0;i<THREADS;i++)
        pthread_join(th[i],NULL);

    t_mix=now_sec()-start;


    printf("Image size: %d x %d\n",input.w,input.h);
    printf("Threads used: %d\n\n",THREADS);

    printf("Scalar time: %.6f sec\n",t_scalar);
    printf("SIMD time: %.6f sec\n",t_simd);
    printf("Multithreading time: %.6f sec\n",t_mt);
    printf("Multithreading + SIMD time: %.6f sec\n\n",t_mix);

    printf("Verification: %s\n",
           check_gray(&output)?"PASSED":"FAILED");


    FILE* f=fopen("gray_output.ppm","wb");

    fprintf(f,"P6\n%d %d\n255\n",
            output.w,
            output.h);

    fwrite(output.buf,
           sizeof(RGB),
           output.w*output.h,
           f);

    fclose(f);

    free(input.buf);
    free(output.buf);

    return 0;
}
