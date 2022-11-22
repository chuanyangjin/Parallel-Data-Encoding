#include "global.h"

// Run-length encoding
chunk *encode(chunk* input)
{   
    chunk* output = (chunk *) malloc(sizeof(input));
    output->string = (unsigned char*) malloc(sizeof(unsigned char) * 4096);
    int j = 0;
    int n;
    for (int i = 0; i < input->size; i++) {
        // Record the char
        output->string[j++] = input->string[i];
 
        // Count occurrences
        n = 1;
        while (i + 1 < input->size && input->string[i] == input->string[i + 1]) {
            n++;
            i++;
        }

        // Record the count
        output->string[j++] = (unsigned char) n;
    }
    output->string[j] = '\0';

    output->position = input->position;
    output->size = j;
    return output;
}