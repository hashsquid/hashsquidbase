#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "sha3.h"

static int hashSum(uint8_t b[32] ) {
    int s;
    unsigned i;
    for(i=0; i<32; i++) {
	    s += (int)b[i];
    }
    return s;

   }

static void byte_to_hex(uint8_t b, char s[23]) {
    unsigned i=1;
    s[0] = s[1] = '0';
    s[2] = '\0';
    while(b) {
        unsigned t = b & 0x0f;
        if( t < 10 ) {
            s[i] = '0' + t;
        } else {
            s[i] = 'a' + t - 10;
        }
        i--;
        b >>= 4;
    }
}

static void findHash(int guardSum, unsigned char *val ) { 


    //display byte (0-255) values of baseHash
    // val contains baseHash plus extra space for another hash at the end
    printf("0x");
    for(size_t count = 0; count < 64; count++)
        printf("%02x", val[count]);
    printf("\n");

    unsigned i;


    //get hashSum
    int s;
    s = hashSum(val);
    printf("\n");
    printf("%d",s);
    printf("\n");
    

    //define output keccak256 array
    uint8_t buf[32];

    //initialize first value to  a random number 0-255
    int r = rand();
    buf[0] = r;

    while (guardSum < s) {

	    //recycle last hash product rather than generate a random byte array
	    for(i=0; i<32; i++) {
		    val[i+32] = buf[i];
	    }

	    //get keccak256 hash of the concatenation of baseHash with candidate hash
	    sha3_HashBuffer(256, SHA3_FLAGS_KECCAK, val, 64, buf, sizeof(buf));

	    //helper printing to see what the concatenated baseHash + candidate hash looks like
	   /* 
	    for(i=0; i<64; i++) {
		    char s[6];
		    byte_to_hex(val[i],s);
		    printf("%s",s);
		}
	    printf("\n");
	    */

	    //get sum of keccak256(baseHash + candidate hash)
	    s = hashSum(buf);
	    //printf("%i\n",s);

    }

    printf("best hashSum: %i\n",s);
    printf("best hash: ");

    //display winning hash as hex string
    for(i=0; i<32; i++) {
	    char s[6];
	    byte_to_hex(val[i+32],s);
	    printf("%s",s);

    }
    printf("\n");

}

int main(int argc, char *argv[]) {
	char *p;
	int sumToBeat;
	long conv = strtol(argv[1], &p, 10);
	sumToBeat = conv;
	printf("%d\n",sumToBeat);

	int i;
	char *chararray = argv[2], *pos = chararray;

    unsigned char val[64];
    for (size_t count = 0; count < sizeof val/sizeof *val; count++) {
        sscanf(pos, "%2hhx", &val[count]);
        pos += 2;
    }


	    for(i=0; i<32; i++) {
		    printf("%d\n",val[i]);
	    }


	findHash(sumToBeat, val);
}
