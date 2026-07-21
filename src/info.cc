#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <hb.hh>

#define CHARS 10000

std::string lookup_names[8192];
int num_lookups = 0;
std::string last_command = "s";

static void trace(hb_buffer_t *buffer,
    const char* type,
    const unsigned int lookup_index,
    const unsigned int parent_index,
    const unsigned int index
)
{
  std::string name = lookup_names[lookup_index];
  if (name == "") name = std::to_string (lookup_index);
  while (true)
  {
    printf ("%s %s at %u (%u relative to parent) ", type, name.c_str(), index, index-parent_index);
    std::string text;
    std::getline (std::cin, text);
    if (text.empty ()) { text = last_command; }
    else
    {
      last_command = text;
    }

    unsigned window_start, window_end;
    if (sscanf(text.c_str(), "p %u %u", &window_start, &window_end))
    {
      unsigned int glyph_count;
      hb_buffer_get_glyph_infos (buffer, &glyph_count);
      for (unsigned int i = window_start; i < window_end && i < glyph_count; i++)
      {
	if (i == index) std::cout << "*";
	printf ("%u ", hb_buffer_get_index (buffer, i).codepoint);
      }
      printf ("\n");
    }
    else if (text == "s") { return; }
    else if (text == "n") {
      hb_buffer_set_debug (buffer, false, false);
      return;
    }
    else if (text == "c") {
      hb_buffer_set_debug (buffer, false, true);
      return;
    }
    else {
      int lookup = 0;
      for (int i = 0; i < num_lookups; i++) {
        if (lookup_names[i] == text) {
            lookup = i;
	    break;
	}
      }
      if (lookup == 0) { lookup = stoi (text); }
      hb_buffer_set_debug (buffer, false, true);
      hb_buffer_set_message_func (buffer, trace, lookup);
      return;
    }
  }
}

int main(int argc, char *argv[]) {
    std::ifstream lookup_names_file("C:/Users/11dli/Carmack/carmack/target/lookup_names.txt");
    int lookup = 0;
    std::string lookup_name = argv[1];
    while (std::getline (lookup_names_file, lookup_names[num_lookups]))
    {
      if (lookup_names[num_lookups] == lookup_name) { lookup = num_lookups; }
      num_lookups++;
    }
    if (lookup == 0) { lookup = stoi (lookup_name); }

    hb_blob_t *blob = hb_blob_create_from_file("C:/Users/11dli/Carmack/carmack/target/Carmack.otf");
    hb_face_t *face = hb_face_create(blob, 0);
    hb_font_t *font = hb_font_create(face);

    char input[CHARS+1];
    memset(input, 'a', CHARS);
    input[CHARS] = 0;

    hb_buffer_t *buffer = hb_buffer_create();
    hb_buffer_add_utf8(buffer, input, -1, 0, -1);
    hb_buffer_guess_segment_properties(buffer);
    hb_buffer_set_message_func (buffer, trace, lookup);

    hb_shape(font, buffer, NULL, 0);
    
    hb_buffer_destroy(buffer);
    hb_font_destroy(font);
    hb_face_destroy(face);
    hb_blob_destroy(blob);
    return 0;
}
// Useful commands:
// ninja -C builddir
// builddir/src/info.exe {lookup} {window}
	