#include <stdlib.h>

#include <memory>

struct GlobalAlloc {
	  void *p;
	    GlobalAlloc() {
		        p = malloc(1024);
			  }

	      ~GlobalAlloc() {
		          free(p);
			    }
};

GlobalAlloc g;

int main(int argc, char *argv[])
{
	  (void)argc;
	    (void)argv;


	      char *array = new char[4096];


	        (void)array;
		  std::unique_ptr<char[]> pchar(new char[512]);
		    return 0;
}

