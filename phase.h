#ifndef PHASE_H
#define PHASE_H

#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <sys/stat.h>
#include <unistd.h>
#include "init.h"
#include "rand.h"
#include "bloom.h"
#include "murmurhash2.h"

typedef struct
{
    mpz_t n;
    mpz_t d;
} Private;

typedef struct
{
    mpz_t n;
    mpz_t e;
} Public;

void _server_base(Private *a, Public *b)
{
    gmp_randstate_t state;
    seed_init(state);
    if (a != NULL && b != NULL)
    {
        rsa_init(a->n, b->e, a->d, state);
        mpz_set(b->n, a->n);
    }
    else
        printf("_server_base 'a' or 'b' is NULL\n");
    FILE *fp = fopen("./resource/pub-n.key","wb");
    char p[1024];
    memset(p,0,1024);
    mpz_get_str(p,16,b->n);
    fwrite(p,1,1024,fp);
    fclose(fp);
    fp = fopen("./resource/pub-e.key","wb");
    memset(p,0,1024);
    mpz_get_str(p,16,b->e);
    fwrite(p,1,1024,fp);
    fclose(fp);

    return;
}

void _client_base(Public *b, int Nc, char *rFile)
{
    if (b == NULL)
    {
        printf("_client_base: 'a' is NULL\n");
        return;
    }
    int i = 0;
    gmp_randstate_t state;
    seed_init(state);
    mpz_t tmp, r, r_inv, r_exp;
    mpz_inits(tmp, r, r_inv, r_exp, NULL);
    FILE *fp = fopen(rFile, "w");
    for (i = 0; i < Nc; i++)
    {
        while (1)
        {
            // default r's length is 1024. it could be modified.
            generator_rand(r, 1024, state);
            mpz_gcd(tmp, r, b->n);
            if (mpz_cmp_ui(tmp, 1) == 0)
                break;
        }
        mpz_invert(r_inv, r, b->n);
        mpz_powm(r_exp, r, b->e, b->n);
        fputs(mpz_get_str(NULL, 16, r), fp), fputc(',', fp);
        fputs(mpz_get_str(NULL, 16, r_inv), fp), fputc(',', fp);
        fputs(mpz_get_str(NULL, 16, r_exp), fp), fputc('\n', fp);
    }
    mpz_clears(tmp, r, r_inv, r_exp, NULL);
    fclose(fp);
    return;
}

void _server_setup(struct bloom *bf, Private *a, int total, double error, char *xFile)
{
    //data_file stores data in hexadecimal
    FILE *fp = fopen(xFile, "r");
    mpz_t x;
    mpz_inits(x, NULL);
    bloom_init(bf, total, error);
    // default xStr's max-length is 1024,it can be modified
    char xStr[1024] = {0}, *xChar;
    for (int i = 0; i < total; i++)
    {
        fgets(xStr, 1024, fp);
        mpz_set_str(x, xStr, 16);
        mpz_powm(x, x, a->d, a->n);
        xChar = mpz_get_str(NULL, 16, x);
        bloom_add(bf, xChar, strlen(xChar));
        if (xChar != NULL)
            free(xChar);
    }
    fclose(fp);
    mpz_clears(x, NULL);
    return;
}

//'data_file' stores 'yi', 'r' stores 'r, r_inv, r_exp', 'dest' is used to store 'A[i]=yi*r_exp'
void _client_online_send(Public *b, int Nc, char *yFile, char *rFile, char *aFile)
{
    FILE *fp1 = fopen(yFile, "r");
    FILE *fp2 = fopen(rFile, "r");
    FILE *fp3 = fopen(aFile, "w");
    mpz_t A, y, r;
    mpz_inits(A, y, r, NULL);
    // here '1024' could be modified
    char yStr[1024], rStr1[1024], *rStr2, *aStr;
    for (int i = 0; i < Nc; i++)
    {
        fgets(yStr, 1024, fp1);
        fgets(rStr1, 1024, fp2);
        rStr2 = rStr1;
        rStr2 = strtok(rStr2, ",");
        rStr2 = strtok(NULL, ","), rStr2 = strtok(NULL, ",");
        mpz_set_str(y, yStr, 16);
        mpz_set_str(r, rStr2, 16);
        mpz_mul(A, y, r);
        mpz_mod(A, A, b->n);
        aStr = mpz_get_str(NULL, 16, A);
        fputs(aStr, fp3), fputc('\n', fp3);
        if (aStr != NULL)
            free(aStr);
    }
    fclose(fp1), fclose(fp2), fclose(fp3);
    mpz_clears(A, y, r, NULL);
    return;
}

void _server_online(Private *a, int Nc, char *aFile, char *bFile)
{
    FILE *fp1 = fopen(aFile, "r");
    FILE *fp2 = fopen(bFile, "w");
    char aStr[1024] = {0}, *bStr;
    mpz_t A, B;
    mpz_inits(A, B, NULL);
    for (int i = 0; i < Nc; i++)
    {
        fgets(aStr, 1024, fp1);
        mpz_set_str(A, aStr, 16);
        mpz_powm(B, A, a->d, a->n);
        bStr = mpz_get_str(NULL, 16, B);
        fputs(bStr, fp2), fputc('\n', fp2);
    }
    mpz_clears(A, B, NULL);
    fclose(fp1), fclose(fp2);
    return;
}

void _client_online_receive(Public *b, int Nc, struct bloom *bf, char *bFile, char *rFile, char *yFile, char *sFile)
{
    FILE *fp1 = fopen(bFile, "r");
    FILE *fp2 = fopen(rFile, "r");
    FILE *fp3 = fopen(yFile, "r");
    FILE *fp4 = fopen(sFile, "w");
    mpz_t B, C, r;
    mpz_inits(B, C, r, NULL);
    char bStr[1024], *cStr, rStr1[1024], *rStr2, yStr[1024];
    for (int i = 0; i < Nc; i++)
    {
        fgets(bStr, 1024, fp1);
        fgets(rStr1, 1024, fp2);
        fgets(yStr, 1024, fp3);
        rStr2 = rStr1;
        rStr2 = strtok(rStr2, ",");
        rStr2 = strtok(NULL, ",");
        mpz_set_str(B, bStr, 16);
        mpz_set_str(r, rStr2, 16);
        mpz_mul(C, B, r);
        mpz_mod(C, C, b->n);
        cStr = mpz_get_str(NULL, 16, C);
        if (bloom_check(bf, cStr, strlen(cStr)))
            fputs(yStr, fp4);
        if (cStr != NULL)
            free(cStr);
    }
    fclose(fp1), fclose(fp2), fclose(fp3), fclose(fp4);
    return;
}

void _server_update(struct bloom *bf, Private *a, char *uFile, char *pFile, int Nu)
{
    FILE *fp1 = fopen(uFile, "r");
    FILE *fp2 = fopen(pFile, "w");
    char uStr[1024], *uChar, *pChar;
    mpz_t u;
    mpz_inits(u, NULL);
    register unsigned int c, d, x;
    for (int i = 0; i < Nu; i++)
    {
        fgets(uStr, 1024, fp1);
        mpz_set_str(u, uStr, 16);
        mpz_powm(u, u, a->d, a->n);
        uChar = mpz_get_str(NULL, 16, u);
        int hits = 0;
        c = murmurhash2(uChar, strlen(uChar), 0x9747b28c);
        d = murmurhash2(uChar, strlen(uChar), c);
        for (unsigned int j = 0; j < bf->hashes; j++)
        {
            x = (c + j * d) % bf->bits;
            fprintf(fp2, "%d ", x);
        }
        if (uChar != NULL)
            free(uChar);
    }
    fclose(fp1), fclose(fp2);
    mpz_clears(u, NULL);
    return;
}

void _client_update(struct bloom *bf, char *pFile)
{
    FILE *fp = fopen(pFile, "r");
    int pos;
    while (fscanf(fp, "%d", &pos) != -1)
    {
        test_bit_set_bit(bf->bf, pos, 1);
    }
    fclose(fp);
    return;
}

void _bloom_write(struct bloom *bf, char *file)
{
    FILE *fp = fopen(file, "w");
    cJSON *cbf = cJSON_CreateObject();
    cJSON *tmp = cJSON_CreateArray();
    for (int i = 0; i < bf->bytes; i++)
        cJSON_AddNumberToObject(tmp, "...", bf->bf[i]);
    cJSON_AddNumberToObject(cbf, "entries", bf->entries);
    cJSON_AddNumberToObject(cbf, "error", bf->error);
    cJSON_AddNumberToObject(cbf, "bits", bf->bits);
    cJSON_AddNumberToObject(cbf, "bytes", bf->bytes);
    cJSON_AddNumberToObject(cbf, "hashes", bf->hashes);
    cJSON_AddNumberToObject(cbf, "bpe", bf->bpe);
    cJSON_AddItemToObject(cbf, "bf", tmp);
    cJSON_AddNumberToObject(cbf, "ready", bf->ready);
    char *str = cJSON_Print(cbf);
    fputs(str, fp);
    free(str);
    fclose(fp);
    return;
}

void _bloom_read(struct bloom *bf, char *file)
{
    FILE *fp = fopen(file, "rb");
    int filesize = 0;
    struct stat buf;
    stat(file, &buf);
    filesize = buf.st_size;
    char *str = (char *)malloc(sizeof(char) * filesize);
    fread(str, 1, filesize, fp);
    cJSON *root = cJSON_Parse(str), *tmp, *t;
    tmp = cJSON_GetObjectItem(root, "bits");
    bf->bits = tmp->valueint;
    tmp = cJSON_GetObjectItem(root, "bpe");
    bf->bpe = tmp->valuedouble;
    tmp = cJSON_GetObjectItem(root, "bytes");
    bf->bytes = tmp->valueint;
    tmp = cJSON_GetObjectItem(root, "entries");
    bf->entries = tmp->valueint;
    tmp = cJSON_GetObjectItem(root, "error");
    bf->error = tmp->valuedouble;
    tmp = cJSON_GetObjectItem(root, "hashes");
    bf->hashes = tmp->valueint;
    tmp = cJSON_GetObjectItem(root, "ready");
    bf->ready = tmp->valueint;
    bf->bf = (unsigned char *)malloc(sizeof(char) * bf->bytes);
    t = cJSON_GetObjectItem(root, "bf");
    for (int i = 0; i < bf->bytes; i++)
    {
        tmp = cJSON_GetArrayItem(t, i);
        bf->bf[i] = tmp->valueint;
    }
    free(str);
    cJSON_Delete(root);
    fclose(fp);
    return;
}


/*
int main()
{
    Private a;
    Public b;
    mpz_inits(a.n, a.d, b.e, b.n, NULL);
    gmp_randstate_t state;
    seed_init(state);
    rsa_init(a.n, b.e, a.d, state);
    mpz_set(b.n, a.n);
    struct bloom bf;

    _server_base(&a, &b);
    _client_base(&b, 1000, "r.txt");

    _server_setup(&bf, &a, 1000, 0.001, "x.txt");

    _client_online_send(&b, 1000, "y.txt", "r.txt", "a.txt");
    _server_online(&a, 1000, "a.txt", "b.txt");
    _client_online_receive(&b, 1000, &bf, "b.txt", "r.txt", "y.txt", "s.txt");

    _server_update(&bf, &a, "u.txt", "p.txt", 5);
    _client_update(&bf, "p.txt");

    _client_base(&b, 1003, "r.txt");
    _client_online_send(&b, 1003, "y-.txt", "r.txt", "a.txt");
    _server_online(&a, 1003, "a.txt", "b.txt");
    _client_online_receive(&b, 1003, &bf, "b.txt", "r.txt", "y-.txt", "s.txt");

    return 0;
}
*/
#endif
