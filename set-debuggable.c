/*
 * Copyright (C) Barebit s.r.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "set-debuggable.h"

static const char filename[] = "AndroidManifest.xml";

static struct ResChunk_header chunk;
//static struct ResXMLTree_node node;
static struct ResXMLTree_attrExt attrExt;
static struct ResXMLTree_attribute attribute;

static void success (FILE *stream)
{
  fclose (stream);
  exit (EXIT_SUCCESS);
}

static void failure (FILE *stream)
{
  fclose (stream);
  exit (EXIT_FAILURE);
}

static void fseekx (FILE *stream, long offset, int origin)
{
  if (fseek (stream, offset, origin) != 0) {
    printf ("error: fseek()\n");
    failure (stream);
  }
}

static void freadx (void *buffer, size_t size, FILE *stream)
{
  if (fread (buffer, size, 1, stream) != 1) {
    printf ("error: fread()\n");
    failure (stream);
  }
}

int main ()
{
  unsigned node_bytes_read = 0;
  unsigned xml_bytes_total;
  unsigned debuggable_attr_index;
  unsigned debuggable_attr_found = 0;

  FILE *file = fopen (filename, "rb+");
  if (file == 0) {
    printf ("error: couldn't open %s\n", filename);
    return EXIT_FAILURE;
  }

  freadx (&chunk, sizeof (chunk), file);

  if (chunk.type != RES_XML_TYPE) {
    printf ("error: unexpected header type (not XML resource type)\n");
    failure (file);
  }

  if (chunk.headerSize != sizeof (chunk)) {
    printf ("error: unexpected header size\n");
    failure (file);
  }

  xml_bytes_total = chunk.size;
  node_bytes_read = ftell (file);

  while (node_bytes_read < xml_bytes_total) {
    freadx (&chunk, sizeof (chunk), file);

    if (chunk.type == RES_XML_RESOURCE_MAP_TYPE) {
      unsigned dwords_read;
      unsigned dwords_total = (chunk.size - chunk.headerSize) / sizeof (uint32_t);
      uint32_t item;
      printf ("RESOURCE_MAP: (offset %08X, type %08X, size %08X)\n", node_bytes_read, chunk.type, chunk.size);

      for (dwords_read = 0; dwords_read < dwords_total; dwords_read++) {
        freadx (&item, sizeof (uint32_t), file);
        printf ("[%04X] %08X", dwords_read, item);
        if (item == DEBUGGABLE_ATTR) {
          debuggable_attr_found = 1;
          debuggable_attr_index = dwords_read;
          printf (": DEBUGGABLE_ATTR");
        }
        printf ("\n");
      }

      if (!debuggable_attr_found) {
        printf ("error: debuggable attribute not found (not indicated in resource map)\n");
        failure (file);
      }
    }

    else if (chunk.type == RES_XML_START_ELEMENT_TYPE) {
      unsigned x;

      // it is expected that RESOURCE_MAP has been processed up to this point

      if (!debuggable_attr_found) {
        printf ("error: RESOURCE_MAP not found prior to START_ELEMENT\n");
        failure (file);
      }

      // move to extended info
      fseekx (file, chunk.headerSize - sizeof (chunk), SEEK_CUR);

      // attrExt node with attribute information follows
      freadx (&attrExt, sizeof (attrExt), file);
      printf ("START_ELEMENT with %u attribute(s) found\n", attrExt.attributeCount);

      if (attrExt.attributeCount > 0) {
        // move to first attribute
        fseekx (file, attrExt.attributeStart - sizeof (attrExt), SEEK_CUR);

        for (x = 0; x < attrExt.attributeCount; x++) {
          freadx (&attribute, sizeof (attribute), file);

          if (attribute.name.index == debuggable_attr_index) {
            printf ("'debuggable' attribute: name ref %08X, value %08X\n", attribute.name.index, attribute.typedValue.data);
            if (attribute.typedValue.data != 0) {
              printf ("-> the debuggable attribute is already set.\n");
              success (file);
            }
            else {
              uint32_t data = 0xFFFFFFFF;   // value of "true"
              int seek = offsetof (struct ResXMLTree_attribute, typedValue.data) - sizeof (attrExt);
              fseekx (file, seek, SEEK_CUR);
              if (fwrite (&data, sizeof (uint32_t), 1, file) != 1) {
                printf ("error: couldn't write new debuggable attribute value %08X at offset %08X.\n", data, ftell (file));
                failure (file);
              }
              printf ("the debuggable attribute value successfully rewritten to %08X.\n", data);
              success (file);
            }
          }
          else {
            printf ("skipping attribute with name ref %08X, value %08X\n", attribute.name.index, attribute.typedValue.data);
          }
        }
      }
    }

    else {
      printf ("skipping node at offset %08X of type %08X, size %08X\n", node_bytes_read, chunk.type, chunk.size);
      fseekx (file, chunk.size - sizeof (chunk), SEEK_CUR);
    }

    node_bytes_read = ftell (file);
  }

  printf ("error: unexpected manifest format (debuggable attribute indicated in resource map but not really referenced)\n");
  failure (file);
}
