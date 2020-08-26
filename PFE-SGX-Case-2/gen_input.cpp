#include <iostream>
#include <stdlib.h>

using namespace std;

int main(int argc, char *argv[]){
  srand (time(NULL));
  int z = atoi(argv[1]);
  for(int i=0;i<z;i++)
    cout <<(rand()%2);
  cout << endl;
  return 0;
}
