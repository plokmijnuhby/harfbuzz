#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <string>
#include <iostream>
#include <hb.hh>

#define CHARS 2000

unsigned int debugged_lookup_id, window = 0;
int recursion_depth = 0;
int next_recursion_depth = -1;
unsigned int indices[64] = {0};

static hb_bool_t trace(
    hb_buffer_t *buffer, hb_font_t *font, const char *message, void *user_data
) {
    unsigned int lookup, current_index = 0;
    bool entering =
        sscanf_s(message, "recursing to lookup %u at %u", &lookup, &current_index)
        || sscanf_s(message, "start lookup %u", &lookup);
    bool exiting =
        sscanf_s(message, "recursed to lookup %u", &lookup)
        || sscanf_s(message, "end lookup %u", &lookup);
    indices[recursion_depth] = current_index;
    
    if (entering && lookup == debugged_lookup_id) {
        next_recursion_depth = recursion_depth + 1;
    }
    
    if (recursion_depth <= next_recursion_depth) {
        unsigned int glyph_count;
        hb_glyph_info_t *glyph_info = hb_buffer_get_glyph_infos(buffer, &glyph_count);
        unsigned int index = indices[recursion_depth-1];
	for (unsigned int i = index; i < index + window && i < glyph_count; i++) {
            if (i == current_index) std::cout << "*";
            printf("%u ", glyph_info[i].codepoint);
        }
        std::cout << "\n" << message << " ";

        std::string text;
        std::getline(std::cin, text);
        switch (text[0]) {
            case 'c':
                next_recursion_depth = -1;
                break;
            case 'o':
                next_recursion_depth--;
                break;
            case 's':
                next_recursion_depth++;
                break;
            default:
                break;
        }
    }
    
    if (entering) {
        recursion_depth++;
    } else if (exiting) {
        recursion_depth--;
    }

    return true;
}

int main(int argc, char *argv[]) {
    debugged_lookup_id = atoi(argv[1]);
    window = atoi(argv[2]);

    hb_blob_t *blob = hb_blob_create_from_file("C:/Users/11dli/Carmack/carmack/target/Carmack.otf");
    hb_face_t *face = hb_face_create(blob, 0);
    hb_font_t *font = hb_font_create(face);

    char input[CHARS+1];
    memset(input, 'a', CHARS);
    input[CHARS] = 0;

    hb_buffer_t *buffer = hb_buffer_create();
    hb_buffer_add_utf8(buffer, input, -1, 0, -1);
    hb_buffer_guess_segment_properties(buffer);
    hb_buffer_set_message_func(buffer, trace, NULL, NULL);

    hb_shape(font, buffer, NULL, 0);
    
    hb_buffer_destroy(buffer);
    hb_font_destroy(font);
    hb_face_destroy(face);
    hb_blob_destroy(blob);
    return 0;
}
// ninja -Cbuild
// build/src/info.exe {lookup} {window}