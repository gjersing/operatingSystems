// Test that fork2 fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define N  500

void
print(const char *s)
{
  write(1, s, strlen(s));
}

void fork2test(void) {
  int n, pid = 0 ;

  print("fork2 test\n");
  for(n = 0; n < N; n++) {
    pid = fork2(2);
    if(pid > 0)
      wait(0);
    else if(pid == 0)
      exit(0);
    else {
      print("fork2test: FAIL\n");
      exit(1);
    }
  }

  if(wait(0) != -1) {
    print("fork2test: FAIL\n");
    exit(1);
  }

  print("fork2test: OK\n");
}

int
main(void)
{
  fork2test();
  exit(0);
}
