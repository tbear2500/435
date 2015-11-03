#include <cstdlib>
#include <cstdio>


#define NUM_STRINGS 13
#define STR_LENGTH  256
#define DESC_LENGTH 8

bool is_hex_digit(char);
unsigned char get_char_from_hex_str(char *);
unsigned char get_hex_val(char);
int count_true(bool*, int);
int min(int, int);

int main(int argc, char *argv[]) {

    //xor infilename wordlist outfilename
    if (argc != 2) {
//    if (argc != 4) {
        printf("Wrong number of args.\n");
        printf("Xor infile wordlist outfile\n");
        return -1;
    }

    //open the input file to read it
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Couldn't open file; wrong filename?\n");
        return -1;
    }

    //allocate memory for the input strings
    char **input = new char*[NUM_STRINGS];
    unsigned char **bytes = new unsigned char*[NUM_STRINGS];
    bool *useful = new bool[NUM_STRINGS];

    //get the input strings
    for (int i = 0; i < NUM_STRINGS; i++) {
        input[i] = new char[STR_LENGTH];
        //assume there's no error here, because this isn't for anyone else to use
        input[i] = fgets(input[i], STR_LENGTH, fp);
        if (!is_hex_digit(input[i][0])) {
            useful[i] = false;
        } else {
            useful[i] = true;
            bytes[i] = new unsigned char[STR_LENGTH];
        }
    }

    int *num_bytes = new int[NUM_STRINGS];

    //I could do this in the loop above, but for clarity...
    //convert from the characters to their hex values
    //store the numerical value of the hex number in bytes[].
    for (int i = 0; i < NUM_STRINGS; i++) {
        if (useful[i]) {
            for (int j = 0, index = 0; j < STR_LENGTH, index < STR_LENGTH; j++) {
                //input[i] is a char array, so the compiler will automatically
                //multiply index by sizeof(char), I think.
                bytes[i][j] = get_char_from_hex_str(input[i] + index);

                if (input[i][index+2] == '\n') {
                    //j is zero-indexed
                    num_bytes[i] = j + 1;
                    break;
                }

                //two hex digits
                index += 2;
            }
        }
    }

    for (int i = 0; i < NUM_STRINGS; i++) {
        delete[] input[i];
    }
    delete[] input;

    //get number of useful rows, so we can calculate how many XORs to do
    int num_useful = count_true(useful, NUM_STRINGS);

    //calculate number of XORs - it's
    // \sum_{i=1}^{n-1} i, equal to
    // n*(n-1)/2
    //where n is the number of useful rows
    int num_xors = num_useful * (num_useful - 1) / 2;
    unsigned char ** xors = new unsigned char*[num_xors];
    char **xor_desc = new char*[num_xors];
    int *xor_length = new int[num_xors];

    int xor_it = 0;

    //XOR every byte string against every other byte string
    for (int i = 0; i < NUM_STRINGS; i++) {
        if (useful[i]) {
            for (int j = i + 1; j < NUM_STRINGS; j++) {
                if (useful[j]) {
// ^ is bitwise XOR operator
                    //so we can say which two lines are responsible for an xor
                    xor_desc[xor_it] = new char[DESC_LENGTH];
                    sprintf(xor_desc[xor_it], "%d %d", i, j);
                    xor_length[xor_it] = min(num_bytes[i], num_bytes[j]);
                    xors[xor_it] = new unsigned char[xor_length[xor_it]];
                    for (int k = 0; k < xor_length[xor_it]; k++) {
                        xors[xor_it][k] = bytes[i][k] ^ bytes[j][k];
                    }
                    //make sure we increment this, and LAST in this loop
                    xor_it++;
                }
            }
        }
    }

    //print the XOR'd strings for a sanity check
    for (int i = 0; i < num_xors; i++) {
        printf("%s\n", xor_desc[i]);
        for (int j = 0; j < xor_length[i]; j++) {
            printf("%02x ", xors[i][j]);
        }
        printf("\n");
    }



    // Now we have a collection of (1) byte arrays and (2) XOR'd byte arrays
    // We're done with the input file, so close it
    fclose(fp);


    // Let's do some analysis!
    fp = fopen(argv[2], "r");

#define MAX_WORDLENGTH 64
#define NUM_WORDS 10000
    char **words = new char*[NUM_WORDS];
    for (int i = 0; i < NUM_WORDS; i++) {
        words[i] = new char[MAX_WORDLENGTH];
        fgets(words[i], MAX_WORDLENGTH, fp);

        //remove trailing '\n' from every string
        //and make sure it is null-terminated
        int c = 0;
        do {
            if (words[i][c] == '\n') {
                words[i][c] = '\0';
            }
        } while (words[i][c] != '\0');
    }
























    for (int i = 0; i < NUM_STRINGS; i++) {
        delete[] bytes[i];
    }
    for (int i = 0; i < num_xors; i++) {
        delete[] xor_desc;
    }
    delete[] bytes;
    delete[] useful;
    return 0;
}


bool is_hex_digit(char input) {
    return  ((input >= 'a' && input <= 'f') || 
             (input >= 'A' && input <= 'F') ||
             (input >= '0' && input <= '9'));
}

//returns the value of the first two digits
//of a hexadecimal string
unsigned char get_char_from_hex_str(char *string) {
    unsigned char hex_num;
    //error checking, even though it means we're redundantly
    //calling get_hex_val
    if (!is_hex_digit(string[0])) {
        exit(-2);
    }
    if (!is_hex_digit(string[1])) {
        exit(-2);
    }
    hex_num = 16 * get_hex_val(string[0]);
    hex_num += get_hex_val(string[1]);
    return hex_num;
}

//returns the numerical value of a digit given in hexadecimal
unsigned char get_hex_val(char digit) {
    if (digit >= 'a' && digit <= 'f') {
        return digit - 'a' + (char)10;
    } else if (digit >= 'A' && digit <= 'F') {
        return digit - 'A' + (char)10;
    } else if (digit >= '0' && digit <= '9') {
        return digit - '0';
    }
    //it wasn't a valid hex digit
    return (char)-1;
}

int count_true(bool *bit_array, int length) {
    int num_true = 0;
    for (int i = 0; i < length; i++) {
        if (bit_array[i]) {
            num_true++;
        }
    }
    return num_true;
}

int min(int int1, int int2) {
    if (int1 < int2) {
        return int1;
    } else {
        return int2;
    }
}