#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <hb.hh>

#define CHARS 10000

unsigned int window = 0;
std::string lookup_names[8192];

static hb_bool_t trace(hb_buffer_t *buffer,
    const char* type,
    const unsigned int lookup_index,
    const unsigned int parent_index,
    const unsigned int index
) {
    unsigned int glyph_count;
    hb_buffer_get_glyph_infos(buffer, &glyph_count);
    for (unsigned int i = parent_index; i < parent_index + window && i < glyph_count;
        i++)
    {
        if (i == index) std::cout << "*";
        printf("%u ", hb_buffer_get_index(buffer, i).codepoint);
    }
    std::string name = lookup_names[lookup_index];
    if (name == "") name = std::to_string (lookup_index);
    std::cout << "\n" << type << " " << name << " ";

    std::string text;
    std::getline(std::cin, text);
    return text[0] == 's';
}

int main(int argc, char *argv[]) {
    window = atoi(argv[2]);

    std::ifstream lookup_names_file("C:/Users/11dli/Carmack/carmack/target/lookup_names.txt");
    int i = 0;
    while (std::getline (lookup_names_file, lookup_names[i])) i++;

    hb_blob_t *blob = hb_blob_create_from_file("C:/Users/11dli/Carmack/carmack/target/Carmack.otf");
    hb_face_t *face = hb_face_create(blob, 0);
    hb_font_t *font = hb_font_create(face);

    char input[CHARS+1];
    memset(input, 'a', CHARS);
    input[CHARS] = 0;

    hb_buffer_t *buffer = hb_buffer_create();
    hb_buffer_add_utf8(buffer, input, -1, 0, -1);
    hb_buffer_guess_segment_properties(buffer);
    hb_buffer_set_message_func(buffer, trace, atoi(argv[1]));

    hb_shape(font, buffer, NULL, 0);
    
    hb_buffer_destroy(buffer);
    hb_font_destroy(font);
    hb_face_destroy(face);
    hb_blob_destroy(blob);
    return 0;
}
// ninja -Cbuild
// build/src/info.exe {lookup} {window}  
