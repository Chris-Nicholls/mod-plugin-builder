/*
  Copyright 2006-2011 David Robillard <d@drobilla.net>
  Copyright 2006 Steve Harris <steve@plugin.org.uk>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/** Include standard C headers */
#include <math.h>
#include <stdlib.h>

/**
   LV2 headers are based on the URI of the specification they come from, so a
   consistent convention can be used even for unofficial extensions.  The URI
   of the core LV2 specification is <http://lv2plug.in/ns/lv2core>, by
   replacing `http:/` with `lv2` any header in the specification bundle can be
   included, in this case `lv2.h`.
*/
#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

/**
   The URI is the identifier for a plugin, and how the host associates this
   implementation in code with its description in data.  In this plugin it is
   only used once in the code, but defining the plugin URI at the top of the
   file is a good convention to follow.  If this URI does not match that used
   in the data files, the host will fail to load the plugin.
*/
#define TIM_URI "http://lv2plug.in/plugins/eg-tim"

/**
   In code, ports are referred to by index.  An enumeration of port indices
   should be defined for readability.
*/
typedef enum {
	TIM_INPUT    = 0,
	TIM_OUTPUT   = 1,
  TIM_GAIN     = 2,
  TIM_VOLUME   = 3,
  TIM_TREBLE   = 4,
  TIM_BASS     = 5,
} PortIndex;

/**
   Every plugin defines a private structure for the plugin instance.  All data
   associated with a plugin instance is stored here, and is available to
   every instance method.  In this simple plugin, only port buffers need to be
   stored, since there is no additional instance data.
*/

typedef struct {
  float prevInput;
  float highPass;
  float postGain;
  float lowPass;
} TimState;

typedef struct {
	// Port buffers
  const float* gain;
  const float* volume;
  const float* treble;
	const float* bass;
	const float* input;
	float*       output;
  double dt;
  TimState* state;
} Tim;

/**
   The `instantiate()` function is called by the host to create a new plugin
   instance.  The host passes the plugin descriptor, stimle rate, and bundle
   path for plugins that need to load additional resources (e.g. waveforms).
   The features parameter contains host-provided features defined in LV2
   extensions, but this simple plugin does not use any.

   This function is in the ``instantiation'' threading class, so no other
   methods on this instance will be called concurrently with it.
*/
static LV2_Handle
instantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               bundle_path,
            const LV2_Feature* const* features)
{
	Tim* tim = (Tim*)malloc(sizeof(Tim));
  TimState* state = (TimState*)malloc(sizeof(TimState));

  tim->state = state;
  tim->dt = 1/rate;
	return (LV2_Handle)tim;
}

/**
   The `connect_port()` method is called by the host to connect a particular
   port to a buffer.  The plugin must store the data location, but data may not
   be accessed except in run().

   This method is in the ``audio'' threading class, and is called in the same
   context as run().
*/
static void
connect_port(LV2_Handle instance,
             uint32_t   port,
             void*      data)
{
	Tim* tim = (Tim*)instance;

	switch ((PortIndex)port) {
	case TIM_GAIN:
		tim->gain = (const float*)data;
		break;
	case TIM_INPUT:
		tim->input = (const float*)data;
		break;
	case TIM_OUTPUT:
		tim->output = (float*)data;
		break;
  case TIM_VOLUME:
    tim->volume = (const float*)data;
    break;
  case TIM_TREBLE:
    tim->treble = (const float*)data;
    break;
  case TIM_BASS:
    tim->bass = (const float*)data;
    break;
	}
}

/**
   The `activate()` method is called by the host to initialise and prepare the
   plugin instance for running.  The plugin must reset all internal state
   except for buffer locations set by `connect_port()`.  Since this plugin has
   no other internal state, this method does nothing.

   This method is in the ``instantiation'' threading class, so no other
   methods on this instance will be called concurrently with it.
*/
static void
activate(LV2_Handle instance)
{
    Tim* tim = (Tim*)instance;
    TimState* state = tim->state;
    state->prevInput = 0;
    state->lowPass = 0;
    state->highPass = 0;
    state->postGain = 0;

}

/**
   The `run()` method is the main process function of the plugin.  It processes
   a block of audio in the audio context.  Since this plugin is
   `lv2:hardRTCapable`, `run()` must be real-time safe, so blocking (e.g. with
   a mutex) or memory allocation are not allowed.
*/
static void
run(LV2_Handle instance, uint32_t n_stimles)
{
	const Tim* tim = (const Tim*)instance;

	const float* const input  = tim->input;
	float* const       output = tim->output;
  
  const float gain   = *(tim->gain);
  const float bass   = *(tim->bass);
  const float treble = *(tim->treble);
  const float volume = *(tim->volume);

  const float highPass_rc = (50000*bass+3370)*1000*1e-9;
  const float highPass_alpha = highPass_rc/(highPass_rc + tim->dt);

  const float lowPass_rc = (50000*treble+1500)*1e-8;
  const float lowPass_alpha = tim->dt/(lowPass_rc + tim->dt);

  TimState* state = tim->state;

	for (uint32_t pos = 0; pos < n_stimles; pos++) {
    float in = input[pos];
    state->highPass = highPass_alpha * (state->highPass + in - state->prevInput);
    state->prevInput = in;
      
    if (abs(state->highPass *gain) < 0.1) {
      state->postGain = state->highPass  * gain;
    }else{
      state->postGain = state->highPass  +  (state->highPass  > 0 ? 0.1: -0.1);
    }
    state->lowPass = lowPass_alpha*state->postGain + (1-lowPass_alpha)*state->lowPass;
		output[pos] = state->lowPass*volume;
	}
}

/**
   The `deactivate()` method is the counterpart to `activate()`, and is called by
   the host after running the plugin.  It indicates that the host will not call
   `run()` again until another call to `activate()` and is mainly useful for more
   advanced plugins with ``live'' characteristics such as those with auxiliary
   processing threads.  As with `activate()`, this plugin has no use for this
   information so this method does nothing.

   This method is in the ``instantiation'' threading class, so no other
   methods on this instance will be called concurrently with it.
*/
static void
deactivate(LV2_Handle instance)
{
}

/**
   Destroy a plugin instance (counterpart to `instantiate()`).

   This method is in the ``instantiation'' threading class, so no other
   methods on this instance will be called concurrently with it.
*/
static void
cleanup(LV2_Handle instance)
{
  free(((Tim*)instance)->state);
	free(instance);
}

/**
   The `extension_data()` function returns any extension data supported by the
   plugin.  Note that this is not an instance method, but a function on the
   plugin descriptor.  It is usually used by plugins to implement additional
   interfaces.  This plugin does not have any extension data, so this function
   returns NULL.

   This method is in the ``discovery'' threading class, so no other functions
   or methods in this plugin library will be called concurrently with it.
*/
static const void*
extension_data(const char* uri)
{
	return NULL;
}

/**
   Every plugin must define an `LV2_Descriptor`.  It is best to define
   descriptors statically to avoid leaking memory and non-portable shared
   library constructors and destructors to clean up properly.
*/
static const LV2_Descriptor descriptor = {
	TIM_URI,
	instantiate,
	connect_port,
	activate,
	run,
	deactivate,
	cleanup,
	extension_data
};

/**
   The `lv2_descriptor()` function is the entry point to the plugin library.  The
   host will load the library and call this function repeatedly with increasing
   indices to find all the plugins defined in the library.  The index is not an
   indentifier, the URI of the returned descriptor is used to determine the
   identify of the plugin.

   This method is in the ``discovery'' threading class, so no other functions
   or methods in this plugin library will be called concurrently with it.
*/
LV2_SYMBOL_EXPORT
const LV2_Descriptor*
lv2_descriptor(uint32_t index)
{
	switch (index) {
	case 0:  return &descriptor;
	default: return NULL;
	}
}
