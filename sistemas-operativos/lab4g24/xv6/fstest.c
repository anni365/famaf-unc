#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

#define NBLOCKS1 100
#define NBLOCKS2 1000

static char test[] = "test";
static char letters1[NBLOCKS1];
static char letters2[NBLOCKS2];

int
memcmp(const void *v1, const void *v2, uint n);

void
fill_data(char *data, char letter, int chunk_size);
char
take_rand_letter(unsigned long *s_next);
void
letters_for_blocks(char *letters, int size);
void
test_create();
void
test_rm();
void
test_write_read(char *letters, int nblocks, int rw_size);
void
test_read(char *letters, int nblocks, int r_size);

int
main(int argc, char *argv[])
{
  // Generamos los caracteres para cada bloque.
  letters_for_blocks(letters1, NBLOCKS1);
  letters_for_blocks(letters2, NBLOCKS2);

  printf(1, "Creando archivo %s: ", test);
  test_create();
  
  printf( 1, "Test 1, writing %d/%d bytes/blocks in chunks of %d bytes:\n"
        , BSIZE*NBLOCKS1, NBLOCKS1, BSIZE);
  test_write_read(letters1, NBLOCKS1, 1);

  printf( 1, "Test 2, reading %d/%d bytes/blocks in chunks of %d bytes:\n"
        , BSIZE*NBLOCKS1, NBLOCKS1, BSIZE);
  test_read(letters1, NBLOCKS1, 1);

  printf(1, "Test 3, borrando test: ");
  test_rm();

  printf( 1, "Test 4, writing %d/%d bytes/blocks in chunks of %d bytes:\n"
        , BSIZE*NBLOCKS2, NBLOCKS2, BSIZE);
  test_write_read(letters2, NBLOCKS2, 1);

  printf( 1, "Test 5, reading %d/%d bytes/blocks in chunks of %d bytes:\n"
        , BSIZE*NBLOCKS2, NBLOCKS2, BSIZE);
  test_read(letters2, NBLOCKS2, 1);

  printf(1, "Test 6, borrando test: ");
  test_rm();

  printf( 1, "Test 7, writing %d/%d bytes/blocks in chunks of %d bytes:\n"
        , BSIZE*NBLOCKS2, NBLOCKS2, BSIZE*4);
  test_write_read(letters2, NBLOCKS2, 4);

  printf( 1, "Test 8, reading %d/%d bytes/blocks in chunks of %d bytes:\n"
        , BSIZE*NBLOCKS2, NBLOCKS2, BSIZE*4);
  test_read(letters2, NBLOCKS2, 4);

  printf(1, "Test 9, borrando test: ");
  test_rm();

  exit();
  return 0;
}

void
test_create ()
{
  int fd;
  
  fd = open(test, O_CREATE | O_WRONLY);
  close(fd);
  
  if (open(test, O_RDONLY) > -1) {
    printf(1, "OK ✓\n");
  } else {
    printf(1, "FAIL ✗\n");
  }
  
  close(fd);
}

void
test_write_read (char *letters, int nblocks, int rw_size)
{
  int rfd, wfd;
  int i, n, progress;
  int nfrags, chunk_size;
  char * mdata = 0;

  nfrags = nblocks / rw_size;
  chunk_size = rw_size*BSIZE;

  mdata = (char*)malloc(chunk_size*sizeof(char));

  // Escribimos el archivo test.
  wfd = open(test, O_CREATE | O_WRONLY);
  
  progress = n = 0;
  printf(1, "Writing\n");
  for(i = 0; i < nfrags; ++i) {
    fill_data(mdata, letters[i], chunk_size);
    n += write(wfd, mdata, chunk_size);
    progress++;
    if (progress % (nfrags/10) == 0)
      printf(2, ".");
  }
  printf(2, "✓\n");
  if (n == BSIZE*nblocks) {
    printf(1, "OK ✓\n");
  } else {
    printf(1, "FAIL ✗ escribimos %d de los %d\n", n, BSIZE*nblocks);
  }

  close(wfd);
  free(mdata);
  mdata = (char*)malloc(chunk_size*sizeof(char));

  // Leémos el archivo test.
  rfd = open(test, O_RDONLY);

  printf(1, "Reading\n");
  progress = n = 0;
  for(i = 0; i < nfrags; ++i) {
    n += read(rfd, mdata, chunk_size);
    progress++;
    if (progress % (nfrags/10) == 0)
      printf(2, ".");
  }
  printf(2, "✓\n");
  close(rfd);
  free(mdata);
  
  // Leímos la misma cantidad que escribimos.
  if (n == BSIZE*nblocks) {
    printf(1, "OK ✓\n");
  } else {
    printf(1, "FAIL ✗: escribimos %d y leímos %d\n", BSIZE*nblocks, n);
  }
}

void
test_read (char *letters, int nblocks, int r_size)
{
  int rfd;
  int i, n, progress;
  int nfrags, chunk_size;
  char *mdata, *rdata;

  nfrags = nblocks / r_size;
  chunk_size = r_size*BSIZE;

  mdata = (char*)malloc(chunk_size*sizeof(char));
  rdata = (char*)malloc(chunk_size*sizeof(char));
  // Abrimos el archivo test.
  rfd = open(test, O_RDONLY);

  progress = n = 0;
  // Leémos el archivo test.
  for(i = 0; i < nfrags; ++i) {
    fill_data(mdata, letters[i], chunk_size);
    read(rfd, rdata, chunk_size);
    n += memcmp(rdata,mdata,chunk_size);
    if (n != 0)
      break;
    progress++;
    if (progress % (nfrags/10) == 0)
      printf(2, ".");
  }
  printf(2, "✓\n");
  close(rfd);
  free(mdata);
  free(rdata);

  /*
    Todos los fragmentos leídos deberían
    ser iguales.
  */
  if (n == 0) {
    printf(1, "OK ✓\n");
  } else {
    printf(1, "FAIL ✗ error leyendo el fragmento %d\n", i);
  }
}

void test_rm (){

  if (unlink(test) >= 0) {
    printf(1, "OK ✓\n");
  } else {
    printf(1, "FAIL ✗\n");
  }
}

int
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  while(n-- > 0){
    if(*s1 != *s2)
      return *s1 - *s2;
    s1++, s2++;
  }

  return 0;
}

char
take_rand_letter(unsigned long *s_next)
{
  unsigned long next = *s_next;

  next = next * 1103515245 + 12345;
  int rand = ((unsigned)(next/65536) % 32768);

  *s_next = next;
  return (char)((rand % 25) + 97);
}

void
letters_for_blocks(char *letters, int size)
{
  unsigned int i = 0;
  unsigned long seed = uptime();
  for(i = 0; i < size; i++)
    letters[i] = take_rand_letter(&seed);
}

void
fill_data(char *data, char letter, int chunk_size)
{
  memset(data, letter, chunk_size);
}
