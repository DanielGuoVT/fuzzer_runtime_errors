#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "projects.h" // For pj_gc_unloadall()
#include "proj_api.h"


int fuzz(const uint8_t *buf, size_t len)
{
    /* We expect the blob to be 3 lines: */
    /* source proj string\ndestination proj string\nx y */
    char* buf_dup = (char*)malloc(len+1);
    memcpy(buf_dup, buf, len);
    buf_dup[len] = 0;
    char* first_line = buf_dup;
    char* first_newline = strchr(first_line, '\n');
    if( !first_newline )
    {
        free(buf_dup);
        return 0;
    }
    first_newline[0] = 0;
    char* second_line = first_newline + 1;
    char* second_newline = strchr(second_line, '\n');
    if( !second_newline )
    {
        free(buf_dup);
        return 0;
    }
    second_newline[0] = 0;
    char* third_line = second_newline + 1;
    projPJ pj_src = pj_init_plus(first_line);
    if( !pj_src )
    {
        free(buf_dup);
        return 0;
    }
    projPJ pj_dst = pj_init_plus(second_line);
    if( !pj_dst )
    {
        free(buf_dup);
        pj_free(pj_src);
        pj_gc_unloadall(pj_get_default_ctx());
        pj_deallocate_grids();
        return 0;
    }
    double x = 0, y = 0, z = 9;
    int from_binary = 0;
    int has_z = 0;
    if( strncmp(third_line, "BINARY_2D:", strlen("BINARY_2D:")) == 0 &&
        third_line - first_line + strlen("BINARY_2D:") + 2 * sizeof(double) <= len )
    {
        from_binary = 1;
        memcpy(&x, third_line + strlen("BINARY_2D:"), sizeof(double));
        memcpy(&y, third_line + strlen("BINARY_2D:") + sizeof(double), sizeof(double));
    }
    else if( strncmp(third_line, "BINARY_3D:", strlen("BINARY_3D:")) == 0 &&
             third_line - first_line + strlen("BINARY_3D:") + 3 * sizeof(double) <= len )
    {
        from_binary = 1;
        has_z = 1;
        memcpy(&x, third_line + strlen("BINARY_3D:"), sizeof(double));
        memcpy(&y, third_line + strlen("BINARY_3D:") + sizeof(double), sizeof(double));
        memcpy(&z, third_line + strlen("BINARY_3D:") + 2 * sizeof(double), sizeof(double));
    }
    else if( sscanf(third_line, "%lf %lf", &x, &y) != 2 )
    {
        free(buf_dup);
        pj_free(pj_src);
        pj_free(pj_dst);
        pj_gc_unloadall(pj_get_default_ctx());
        pj_deallocate_grids();
        return 0;
    }
#ifdef STANDALONE
    fprintf(stderr, "src=%s\n", first_line);
    fprintf(stderr, "dst=%s\n", second_line);
    if( from_binary )
    {
        if( has_z )
            fprintf(stderr, "coord (from binary)=%.18g %.18g %.18g\n", x, y, z);
        else
            fprintf(stderr, "coord (from binary)=%.18g %.18g\n", x, y);
    }
    else
        fprintf(stderr, "coord=%s\n", third_line);
#endif
    if( has_z )
        pj_transform( pj_src, pj_dst, 1, 0, &x, &y, &z );
    else
        pj_transform( pj_src, pj_dst, 1, 0, &x, &y, NULL );
    free(buf_dup);
    pj_free(pj_src);
    pj_free(pj_dst);
    pj_gc_unloadall(pj_get_default_ctx());
    pj_deallocate_grids();
    return 0;
}

#ifdef STANDALONE

int main(int argc, char** argv)
{
    if( argc < 2 )
    {
        const char str[] =
            "+proj=longlat +datum=WGS84 +nodefs\n+proj=longlat +datum=WGS84 +nodefs\n2 49";
        int ret = fuzz((const uint8_t*)(str), sizeof(str) - 1);
        if( ret )
            return ret;

        const char str2[] =
            "+proj=longlat +datum=WGS84 +nodefs\n+proj=longlat +datum=WGS84 +nodefs\nBINARY_2D:\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF\xFF";
        ret = fuzz((const uint8_t*)(str2), sizeof(str2) - 1);
        if( ret )
            return ret;

        const char str3[] =
            "+proj=longlat +datum=WGS84 +nodefs\n+proj=longlat +datum=WGS84 +nodefs\nBINARY_3D:\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF\xFF\x00\x00\x00\x00\x00\x00\x00\x00";
        ret = fuzz((const uint8_t*)(str3), sizeof(str3) - 1);
        if( ret )
            return ret;

        return 0;
    }
    else
    {
        int nRet = 0;
        void* buf = NULL;
        int nLen = 0;
        FILE* f = fopen(argv[1], "rb");
        if( !f )
        {
            fprintf(stderr, "%s does not exist.\n", argv[1]);
            exit(1);
        }
        fseek(f, 0, SEEK_END);
        nLen = (int)ftell(f);
        fseek(f, 0, SEEK_SET);
        buf = malloc(nLen);
        if( !buf )
        {
            fprintf(stderr, "malloc failed.\n");
            fclose(f);
            exit(1);
        }
        fread(buf, nLen, 1, f);
        fclose(f);
        nRet = fuzz((const uint8_t*)(buf), nLen);
        free(buf);
        return nRet;
    }
}

#endif // STANDALONE
