#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>

#include "libs/quickjs/quickjs-2025-04-26/quickjs.h"
#include "libs/quickjs/quickjs-2025-04-26/quickjs-libc.h"
// #include <map>

// using namespace std;

struct SPI_Memory
{
    unsigned char outgoing_midi[256];
    unsigned char outgoing_random[512];
    unsigned char outgoing_unknown[1280];
    unsigned char incoming_midi[256];
    unsigned char incoming_random[512];
    unsigned char incoming_unknown[1280];
};

unsigned char *mapped_memory;

int outgoing_midi_counter = 0;

struct USB_MIDI_Packet
{
    unsigned char cable;
    unsigned char code_index_number : 4;
    unsigned char midi_0;
    unsigned char midi_1;
    unsigned char midi_2;
};

int queueMidiSend(int cable, unsigned char *buffer, int length)
{
    if (outgoing_midi_counter + length > 256)
    {
        printf("Outgoing MIDI send queue is full. Discarding messages.");
        return 0;
    }

    // int usbPackets = length / 4;
    // int remaining = length % 1;

    // for (int i=0; i<usbPackets; i++) {

    //     struct USB_MIDI_Packet* packet = ((struct SPI_Memory *)mapped_memory)->outgoing_midi[i];

    //     packet->cable = cable;
    //     packet->code_index_number;
    // }

    // ((struct SPI_Memory *)mapped_memory)->outgoing_midi

    // memcpy()

    // memcpy(((struct SPI_Memory *)mapped_memory)->outgoing_midi, LPPInitSysex, 23);
    // int ioctl_result = ioctl(fd, _IOC(_IOC_NONE, 0, 0xa, 0), 0x300);

    // unsigned char out_cable = 2;

    // unsigned char LPPInitSysex[24] = {
    //     (unsigned char)(out_cable << 4 | 0x4), 0xF0, 126, 0,
    //     (unsigned char)(out_cable << 4 | 0x4), 6, 2, 0,
    //     (unsigned char)(out_cable << 4 | 0x4), 32, 41, 0x00,
    //     (unsigned char)(out_cable << 4 | 0x4), 0x00, 0x00, 0x00,
    //     (unsigned char)(out_cable << 4 | 0x4), 0x00, 0x00, 0x00,
    //     (unsigned char)(out_cable << 4 | 0x6), 0x00, 0xF7, 0x0};
    // printf("%d", memcmp(buffer, &LPPInitSysex, 24));

    // buffer[0] = cable << 4 | buffer[0];

    // printf("queueMidi: queueing %d bytes to outgoing MIDI ,counter:%d\n", length, outgoing_midi_counter);
    // memcpy(((struct SPI_Memory *)mapped_memory)->outgoing_midi + outgoing_midi_counter, buffer, length);
    memcpy(((struct SPI_Memory *)mapped_memory)->outgoing_midi + outgoing_midi_counter, buffer, length);

    // int ioctl_result = ioctl(fd, _IOC(_IOC_NONE, 0, 0xa, 0), 0x300);

    outgoing_midi_counter += length;
}

int queueExternalMidiSend(unsigned char *buffer, int length)
{
    return queueMidiSend(2, buffer, length);
}

int queueInternalMidiSend(unsigned char *buffer, int length)
{
    return queueMidiSend(0, buffer, length);
}

void onExternalMidiMessage(unsigned char midi_message[4])
{
    // js_on_external_midi_message()
}

void onInternalMidiMessage(unsigned char midi_message[4])
{
    // js_on_internal_midi_message()
}

void onMidiMessage(unsigned char midi_message[4])
{

    int cable;
    int code_index_number;

    if (cable == 0)
    {
        onInternalMidiMessage(midi_message);
    }

    if (cable == 2)
    {
        onExternalMidiMessage(midi_message);
    }
}

void clearPads(unsigned char *mapped_memory, int fd)
{

    // clear pads
    int j = 0;
    for (int i = 0, pad = 0; pad < 32; pad++)
    {
        j = i * 4;

        ((struct SPI_Memory *)mapped_memory)->outgoing_midi[j + 0] = 0 | 0x9;
        ((struct SPI_Memory *)mapped_memory)->outgoing_midi[j + 1] = 0x90;
        ((struct SPI_Memory *)mapped_memory)->outgoing_midi[j + 2] = (68 + pad);
        ((struct SPI_Memory *)mapped_memory)->outgoing_midi[j + 3] = 0;

        if (i > 9)
        {
            int ioctl_result = ioctl(fd, _IOC(_IOC_NONE, 0, 0xa, 0), 0x300);
            // memset(((struct SPI_Memory *)mapped_memory)->outgoing_midi, 0, 256);
            i = 0;
        }

        // printf("clearing pad %d\n", pad);

        i++;
    }

    int ioctl_result = ioctl(fd, _IOC(_IOC_NONE, 0, 0xa, 0), 0x300);
}

void clearSequencerButtons(unsigned char *mapped_memory, int fd)
{

    // clear pads
    int j = 0;
    for (int i = 0, pad = 0; pad < 16; pad++)
    {
        j = i * 4;

        ((struct SPI_Memory *)mapped_memory)->outgoing_midi[j + 0] = 0 | 0x9;
        ((struct SPI_Memory *)mapped_memory)->outgoing_midi[j + 1] = 0x90;
        ((struct SPI_Memory *)mapped_memory)->outgoing_midi[j + 2] = (16 + pad);
        ((struct SPI_Memory *)mapped_memory)->outgoing_midi[j + 3] = 0;

        if (i > 9)
        {
            int ioctl_result = ioctl(fd, _IOC(_IOC_NONE, 0, 0xa, 0), 0x300);
            // memset(((struct SPI_Memory *)mapped_memory)->outgoing_midi, 0, 256);
            i = 0;
        }

        // printf("clearing button %d\n", pad);

        i++;
    }

    int ioctl_result = ioctl(fd, _IOC(_IOC_NONE, 0, 0xa, 0), 0x300);
}

void kickM8(unsigned char *mapped_memory, int fd)
{
    unsigned char out_cable = 2;

    unsigned char LPPInitSysex[24] = {
        (unsigned char)(out_cable << 4 | 0x4), 0xF0, 126, 0,
        (unsigned char)(out_cable << 4 | 0x4), 6, 2, 0,
        (unsigned char)(out_cable << 4 | 0x4), 32, 41, 0x00,
        (unsigned char)(out_cable << 4 | 0x4), 0x00, 0x00, 0x00,
        (unsigned char)(out_cable << 4 | 0x4), 0x00, 0x00, 0x00,
        (unsigned char)(out_cable << 4 | 0x6), 0x00, 0xF7, 0x0};

    memcpy(((struct SPI_Memory *)mapped_memory)->outgoing_midi, LPPInitSysex, 23);
    int ioctl_result = ioctl(fd, _IOC(_IOC_NONE, 0, 0xa, 0), 0x300);
}

#ifndef FALSE
enum
{
    FALSE = 0,
    TRUE = 1,
};
#endif

/* also used to initialize the worker context */
static JSContext *JS_NewCustomContext(JSRuntime *rt)
{
    JSContext *ctx;
    ctx = JS_NewContext(rt);
    if (!ctx)
        return NULL;
    /* system modules */
    js_init_module_std(ctx, "std");
    js_init_module_os(ctx, "os");
    return ctx;
}

static int eval_buf(JSContext *ctx, const void *buf, int buf_len,
                    const char *filename, int eval_flags)
{
    JSValue val;
    int ret;

    if ((eval_flags & JS_EVAL_TYPE_MASK) == JS_EVAL_TYPE_MODULE)
    {
        /* for the modules, we compile then run to be able to set
           import.meta */
        val = JS_Eval(ctx, buf, buf_len, filename,
                      eval_flags | JS_EVAL_FLAG_COMPILE_ONLY);
        if (!JS_IsException(val))
        {
            js_module_set_import_meta(ctx, val, TRUE, TRUE);
            val = JS_EvalFunction(ctx, val);
        }
        val = js_std_await(ctx, val);
    }
    else
    {
        val = JS_Eval(ctx, buf, buf_len, filename, eval_flags);
    }
    if (JS_IsException(val))
    {
        js_std_dump_error(ctx);
        ret = -1;
    }
    else
    {
        ret = 0;
    }
    JS_FreeValue(ctx, val);
    return ret;
}

static int eval_file(JSContext *ctx, const char *filename, int module)
{
    uint8_t *buf;
    int ret, eval_flags;
    size_t buf_len;

    printf("Loading control surface script: %s\n", filename);
    buf = js_load_file(ctx, &buf_len, filename);
    if (!buf)
    {
        perror(filename);
        exit(1);
    }

    // if (module < 0) {
    //     module = (has_suffix(filename, ".mjs") ||
    //               JS_DetectModule((const char *)buf, buf_len));
    // }
    if (module)
        eval_flags = JS_EVAL_TYPE_MODULE;
    else
        eval_flags = JS_EVAL_TYPE_GLOBAL;
    ret = eval_buf(ctx, buf, buf_len, filename, eval_flags);
    js_free(ctx, buf);
    return ret;
}

// static JSValue js_sum_bytes(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {

#define js_move_midi_external_send_buffer_size 4096
unsigned char js_move_mid_send_buffer[js_move_midi_external_send_buffer_size];
static JSValue js_move_midi_send(int cable, JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv)
{
    // printf("js_move_midi_internal_send %d\n", argc);

    // internal_move_midi_external_send_start();

    int send_buffer_index = 0;

    if (argc != 1)
    {
        JS_ThrowTypeError(ctx, "move_midi_external_send() expects exactly 1 argument, but got %d", argc);
        return JS_EXCEPTION;
    }

    JSValueConst js_array = argv[0];
    if (!JS_IsArray(ctx, js_array))
    {
        JS_ThrowTypeError(ctx, "move_midi_external_send() argument needs to be an Array");
        return JS_EXCEPTION;
    }

    JSValue length_val = JS_GetPropertyStr(ctx, js_array, "length");

    if (JS_IsException(length_val))
    {
        // Should not happen for a valid array
        return JS_EXCEPTION;
    }

    unsigned int len;
    JS_ToUint32(ctx, &len, length_val);
    JS_FreeValue(ctx, length_val);

    printf("[");
    JSValue entry;
    for (int i = 0; i < len; i++)
    {

        JSValue val = JS_GetPropertyUint32(ctx, js_array, i);
        if (JS_IsException(val))
        {
            return JS_EXCEPTION;
        }

        uint32_t byte_val;
        if (JS_ToUint32(ctx, &byte_val, val) != 0)
        {
            JS_FreeValue(ctx, val);
            return JS_ThrowTypeError(ctx, "Array element at index %u is not a number", i);
        }

        if (byte_val > 255)
        {
            JS_FreeValue(ctx, val);
            return JS_ThrowRangeError(ctx, "Array element at index %u (%u) is out of byte range (0-255)", i, byte_val);
        }

        // total_sum += byte_val;
        printf("%d(%x)", byte_val, byte_val);
        if (i != len - 1)
        {
            printf(", ");
        }

        js_move_mid_send_buffer[send_buffer_index] = byte_val;
        send_buffer_index++;

        if (send_buffer_index >= js_move_midi_external_send_buffer_size)
        {
            JS_ThrowInternalError(ctx, "No more space in MIDI internal send buffer.");
            return JS_EXCEPTION;
        }

        JS_FreeValue(ctx, val);
    }

    printf("]\n");

    // flushMidi();
    queueMidiSend(cable, (unsigned char *)js_move_mid_send_buffer, send_buffer_index);
}

static JSValue js_move_midi_external_send(JSContext *ctx, JSValueConst this_val,
                                          int argc, JSValueConst *argv)
{
    js_move_midi_send(2, ctx, this_val, argc, argv);
}

static JSValue js_move_midi_internal_send(JSContext *ctx, JSValueConst this_val,
                                          int argc, JSValueConst *argv)
{
    js_move_midi_send(0, ctx, this_val, argc, argv);
}


void init_javascript(JSRuntime **prt, JSContext **pctx)
{

    JSRuntime *rt;
    JSContext *ctx;
    memset(js_move_mid_send_buffer, 0, sizeof(js_move_mid_send_buffer));

    rt = JS_NewRuntime();
    if (!rt)
    {
        fprintf(stderr, "qjs: cannot allocate JS runtime\n");
        exit(2);
    }

    js_std_set_worker_new_context_func(JS_NewCustomContext);
    js_std_init_handlers(rt);

    ctx = JS_NewCustomContext(rt);
    if (!ctx)
    {
        fprintf(stderr, "qjs: cannot allocate JS context\n");
        exit(2);
    }

    js_std_add_helpers(ctx, -1, 0);
    // char jsCode[] = "console.log('Hello world!')\0";
    // char jsCode[] = "console.log('Hello world!');\0";

    // eval_buf(ctx, jsCode, strlen(jsCode), "<cmdline>", 0);
    JSValue global_obj = JS_GetGlobalObject(ctx);

    JSValue move_midi_external_send_func = JS_NewCFunction(ctx, js_move_midi_external_send, "move_midi_external_send", 1);
    JS_SetPropertyStr(ctx, global_obj, "move_midi_external_send", move_midi_external_send_func);

    JSValue move_midi_internal_send_func = JS_NewCFunction(ctx, js_move_midi_external_send, "move_midi_internal_send", 1);
    JS_SetPropertyStr(ctx, global_obj, "move_midi_internal_send", move_midi_internal_send_func);

    JS_FreeValue(ctx, global_obj);

    // Free our reference to the global object

    // eval_file(ctx, "move_m8.js", -1);

    // JSValue val;
    // val = JS_Eval(ctx, jsCode, strlen(jsCode), "foo.js", 0);
    *prt = rt;
    *pctx = ctx;
}

void getGlobalFunction(JSContext **pctx, const char *func_name, JSValue *retFunc)
{

    JSContext *ctx = *pctx;

    // 4. Get the global object
    JSValue global_obj = JS_GetGlobalObject(ctx);

    // --- Find and Call the 'greet' function ---

    JSValue func = JS_GetPropertyStr(ctx, global_obj, func_name);

    // 5. Check if it's a function
    if (!JS_IsFunction(ctx, func))
    {
        fprintf(stderr, "Error: '%s' is not a function or not found.\n", func_name);
        JS_FreeValue(ctx, func); // Free the non-function value
    }

    *retFunc = func;
}

int callGlobalFunction(JSContext **pctx, JSValue *pfunc, unsigned char *data)
{
    JSContext *ctx = *pctx;
    JSValue newArray;

    // args[0] = JS_NewString(ctx, "foo");
    newArray = JS_NewArray(ctx);

    JSValue num;
    if (!JS_IsException(newArray))
    { // Check creation success

        for (int i = 0; i < 3; i++)
        {
            num = JS_NewInt32(ctx, data[i]);
            JS_SetPropertyUint32(ctx, newArray, i, num);
        }
    }

    JSValue args[1];
    args[0] = newArray;

    JSValue ret = JS_Call(ctx, *pfunc, JS_UNDEFINED, 1, args);

    if (JS_IsException(ret))
    {
        printf("JS function failed\n");
        js_std_dump_error(ctx);
    }

}

void destroy_javascript(JSRuntime **prt, JSContext **pctx)
{

    JS_FreeContext(*pctx);
    JS_FreeRuntime(*prt);
}

int main()
{

    JSRuntime *rt = 0;
    JSContext *ctx = 0;
    init_javascript(&rt, &ctx);

    const char *device_path = "/dev/ablspi0.0";
    struct timespec sleep_time;
    sleep_time.tv_sec = 0;
    sleep_time.tv_nsec = 1 * 1000000;

    int fd;

    size_t length = 4096;
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_SHARED;
    off_t offset = 0;

    // Open the device file.
    printf("Opening file\n");
    fd = open(device_path, O_RDWR);
    if (fd == -1)
    {
        perror("open");
        return 1;
    }

    printf("mmaping\n");
    mapped_memory = (unsigned char *)mmap(NULL, length, prot, flags, fd, offset);

    if (mapped_memory == MAP_FAILED)
    {
        perror("mmap");
        close(fd);
        return 1;
    }

    // Clear mapped memory
    printf("Clearing nmapped memory\n");
    memset(mapped_memory, 0, 4096);



    int padIndex = 0;

   

    // int padColor = 1;

    // clearPads(mapped_memory, fd);
    // clearSequencerButtons(mapped_memory, fd);

    // kickM8(mapped_memory, fd);
    // int knobs[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    // int sysexCounter = 0;
    // unsigned char sysex_buffer[8192];


  /*  // The lighting of white and RGB LEDs is controlled by note-on or control change messages sent to Push 2:

Note On (nn):        1001cccc 0nnnnnnn 0vvvvvvv        [10010000 = 0x90 = 144]
Control Change (cc): 1011cccc 0nnnnnnn 0vvvvvvv        [10110000 = 0xB0 = 176]
The channel (cccc, 0…15) controls the LED animation, i.e. blinking, pulsing or one-shot transitions. Channel 0 means no animation. See LED Animation.

The message type 1001 (for nn) or 1011 (for cc) and the note or controller number nnnnnnn (0…127) select which LED is addressed. See MIDI Mapping.

The velocity vvvvvvv (0…127) selects a color index, which is interpreted differently for white and RGB LEDs. See Default Color Palettes (subset). 
*/

    /*
        CIN     MIDI_x Size     Description
        0x0     1, 2 or 3       Miscellaneous function codes. Reserved for future extensions.
        0x1     1, 2 or 3       Cable events. Reserved for future expansion.
        0x2     2               Two-byte System Common messages like MTC, SongSelect, etc.
        0x3     3               Three-byte System Common messages like SPP, etc.
        0x4     3               SysEx starts or continues
        0x5     1               Single-byte System Common Message or SysEx ends with following single byte.
        0x6     2               SysEx ends with following two bytes.
        0x7     3               SysEx ends with following three bytes.
        0x8     3               Note-off
        0x9     3               Note-on
        0xA     3               Poly-KeyPress
        0xB     3               Control Change
        0xC     2               Program Change
        0xD     2               Channel Pressure
        0xE     3               PitchBend Change
        0xF     1               Single Byte



        currentOutput.send([0xF0, 126, 0, 6, 2, 0, 32, 41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF7]);

    */

    enum
    {
        SYSEX_START_OR_CONTINUE = 0x4,
        SYSEX_END_SINGLE_BYTE = 0x5,
        SYSEX_END_TWO_BYTE = 0x6,
        SYSEX_END_THREE_BYTE = 0x7,
        NOTE_OFF = 0x8,
        NOTE_ON = 0x9,
        POLY_KEYPRESS = 0xA,
        CONTROL_CHANGE = 0xB,
        PROGRAM_CHANGE = 0xC,
        CHANNEL_PRESSURE = 0xD,
        PITCH_BEND = 0xE,
        SINGLE_BYTE = 0xF
    };

    // int prev_code_index_number = -1;

    // memset(((struct SPI_Memory *)mapped_memory)->outgoing_midi, 0, 256);

    int ioctl_result = 0;
    ioctl_result = (fd, _IOC(_IOC_NONE, 0, 0xb, 0), 0x1312d00);

    clearPads(mapped_memory, fd);
    clearSequencerButtons(mapped_memory, fd);

    eval_file(ctx, "move_m8.js", -1);

     JSValue onMidiMessageExternal;
    getGlobalFunction(&ctx, "onMidiMessageExternal", &onMidiMessageExternal);

    JSValue onMidiMessageInternal;
    getGlobalFunction(&ctx, "onMidiMessageInternal", &onMidiMessageInternal);


    while (1)
    {
        // printf("\033[H\033[J");

        ioctl_result = ioctl(fd, _IOC(_IOC_NONE, 0, 0xa, 0), 0x300);
        outgoing_midi_counter = 0;

        int startByte = 2048;
        int length = 256;
        int endByte = startByte + length;

        // padIndex++;
        int control_change = 0xb;

        int out_index = 0;

        memset(((struct SPI_Memory *)mapped_memory)->outgoing_midi, 0, 256);
        for (int i = startByte; i < endByte; i += 4)
        {
            if ((unsigned int)mapped_memory[i] == 0)
            {
                continue;
            }

            unsigned char *byte = &mapped_memory[i];
            unsigned char cable = *byte >> 4;
            unsigned char code_index_number = (*byte & 0b00001111);
            unsigned char midi_0 = *(byte + 1);
            unsigned char midi_1 = *(byte + 2);
            unsigned char midi_2 = *(byte + 3);

            // struct USB_MIDI_Packet* packet = (struct USB_MIDI_Packet*)&mapped_memory[i];

            if (byte[1] + byte[2] + byte[3] == 0)
            {
                continue;
            }

            // if (prev_code_index_number != SYSEX_START_OR_CONTINUE && code_index_number == SYSEX_START_OR_CONTINUE) {
            //     sysexCounter = 0;
            // }

            // if (code_index_number == SYSEX_START_OR_CONTINUE) {
            //     sysex_buffer[0] = byte[1];
            //     sysex_buffer[0] = byte[2];
            //     sysex_buffer[0] = byte[3];
            //     continue;
            // }

            if (cable == 2)
            {
                callGlobalFunction(&ctx, &onMidiMessageExternal, &byte[1]);
                
            }

            if (cable == 0)
            {
                callGlobalFunction(&ctx, &onMidiMessageInternal, &byte[1]);
            }

            // printf("cable: %x,\tcode index number:%x,\tmidi_0:%x,\tmidi_1:%x,\tmidi_2:%x\n", cable, code_index_number, midi_0, midi_1, midi_2);

            // Let's ignore incoming sysex for now
            // if (code_index_number >= NOTE_OFF) {
            //     if (cable == 0)
            //     {
            //         onInternalMidiMessage(byte);
            //     }

            //     if (cable == 2) {
            //         onExternalMidiMessage(byte);
            //     }

            //     continue;
            // }

            // if ((code_index_number == 2 || code_index_number == 1) || (cable == 0xf && code_index_number == control_change && midi_0 == 176))
            // {
            //     continue;
            // })

            // printf("first_byte%d,\tmidi_0:%x,\tmidi_1:%d,\tmidi_2:%d\n", byte, midi_0, midi_1, midi_2);

            // swap input channels
            // if (cable == 2) {
            //     cable = 0;
            // }

            // if (cable == 0) {
            //     cable = 2;
            // }

            // This works

            // LPP input to Move pad
            // if (byte[1] == 0x90 && cable == 2)
            // {
            //     int oi = out_index;
            //     ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 0] = 0 | 0x9;
            //     ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 1] = 0x90;
            //     ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 2] = lppPadToMovePad[byte[2]];
            //     printf("incoming pad: %d, outgoing pad: %d ", byte[2], lppPadToMovePad[byte[2]]);
            //     // ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 3] = padColor % 0x7f;
            //     ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 3] = byte[3];
            //     // padColor++;

            //     out_index++;
            // }

            // // Move note in
            // if (cable == 0 && (byte[1] == 0x90 || byte[1] == 0x80))
            // {
            //     unsigned char isNoteOn = byte[1] == 0x90;

            //     int channel = 1 - 1;
            //     int out_cable = 2;

            //     unsigned char note = byte[2];
            //     unsigned char velocity = byte[3];

            //     unsigned char noteIsNotCapacitiveTouch = note > 9;

            //     if (noteIsNotCapacitiveTouch)
            //     {
            //         int oi = out_index;

            //         ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 0] = out_cable << 4 | (byte[1] / 0x10);
            //         ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 1] = byte[1] | channel;
            //         ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 2] = byte[2];
            //         ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 3] = isNoteOn ? 100 : 0;
            //         printf("Sending Note On %x %x %x %d\n", cable << 4 | byte[1] / 0x10, byte[1] | channel, byte[2], isNoteOn ? 100 : 0);

            //         out_index++;
            //     }
            // }

            // if (byte[1] == 0x80 && cable == 0)
            // {
            //     int channel = 1 - 1;
            //     int out_cable = 2;

            //     int oi = out_index;

            //     ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 0] = out_cable << 4 | 0x8;
            //     ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 1] = 0x80 | channel;
            //     ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 2] = byte[2];
            //     ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 3] = 100;
            //     printf("Sending Note Off %x %x %x %d\n", cable << 4 | 0x8, 0x80 | channel, byte[2], 100);

            //     out_index++;
            // }

            // if (byte[1] == 0xb0 && cable == 0)
            // {
            //     int channel = 1 - 1;
            //     int out_cable = 2;

            //     int oi = out_index;

            //     int knob = -1;
            //     // If this is a Move knob, turn it into a 0-127 value vs +/- 1 values so we can map ot to things on the M8
            //     if (byte[2] >= 71 && byte[2] <= 79)
            //     {
            //         knob = byte[2] - 71;
            //     }

            //     if (knob != -1)
            //     {

            //         if (byte[3] == 127)
            //         {
            //             knobs[knob] -= 1;
            //         }

            //         if (byte[3] == 1)
            //         {
            //             knobs[knob] += 1;
            //         }

            //         if (knobs[knob] > 127)
            //         {
            //             knobs[knob] = 127;
            //         }

            //         if (knobs[knob] < 0)
            //         {
            //             knobs[knob] = 0;
            //         }

            //         ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 0] = out_cable << 4 | 0xb;
            //         ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 1] = 0xb0;
            //         ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 2] = byte[2];
            //         ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 3] = (unsigned char)knobs[knob];
            //         printf("Sending CC to external %x %x %x %d\n", out_cable << 4 | 0xb, 0xb0, byte[2], (unsigned char)knobs[knob]);
            //     }

            //     // Otherwise map from Move controls to LPP pads

            //     if (byte[2] <= 119)
            //     {
            //         unsigned char lppPad = moveControlToLppNote[byte[2]];
            //         unsigned char noteOn = byte[3] == 127;
            //         unsigned char velocity = noteOn ? 100 : 0;
            //         unsigned char messageType = noteOn ? 0x9 : 0x8;

            //         ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 0] = out_cable << 4 | messageType;
            //         ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 1] = messageType * 0x10;
            //         ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 2] = lppPad;
            //         ((struct SPI_Memory *)mapped_memory)->outgoing_midi[oi + 3] = velocity;
            //         printf("Move control to LPP pad %x %x %x %d\n", out_cable << 4 | messageType, messageType * 0x10, lppPad, velocity);
            //     }

            //     out_index++;
            // }

            // printf("out index: %d\n", out_index);
        }
    }

    if (munmap(mapped_memory, length) == -1)
    {
        perror("munmap");
    }

    close(fd);

    printf("Destroying JS");
    destroy_javascript(&rt, &ctx);

    return 0;
}