
#ifndef _KAONSYS_JPEG_H_
#define _KAONSYS_JPEG_H_

#ifdef __cplusplus
extern "C"
{
#endif

GLOBAL(void) write_JPEG_file (unsigned char* image_buffer, int image_width, int image_height, char * filename, int quality);
METHODDEF(void)my_error_exit (j_common_ptr cinfo);
GLOBAL(int)read_JPEG_file (char * filename);

#ifdef __cplusplus
}
#endif


#endif
