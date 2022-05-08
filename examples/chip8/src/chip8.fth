#! /usr/bin/env e4

( ################################################################### )
(                    SDL Bindings and Constants                       )
( ################################################################### )

s\" libSDL2.so\z" drop dlopen value libsdl

\ void SDL_CloseAudioDevice(SDL_AudioDeviceID dev)
ffi-uint32 1 ffi-void
    s\" SDL_CloseAudioDevice\z" drop libsdl dlsym
    ffi-def sdl-close-audio-device

\ SDL_Renderer * SDL_CreateRenderer(SDL_Window * window, int index,
\                                   Uint32 flags)
ffi-pointer ffi-sint ffi-uint32 3 ffi-pointer
    s\" SDL_CreateRenderer\z" drop libsdl dlsym
    ffi-def sdl-create-renderer

\ SDL_Texture * SDL_CreateTexture(SDL_Renderer * renderer, Uint32 format,
\                                 int access, int w, int h)
ffi-pointer ffi-uint32 ffi-sint ffi-sint ffi-sint 5 ffi-pointer
    s\" SDL_CreateTexture\z" drop libsdl dlsym
    ffi-def sdl-create-texture

\ SDL_Window * SDL_CreateWindow(const char *title, int x, int y, int w,
\                               int h, Uint32 flags)
ffi-pointer ffi-sint ffi-sint ffi-sint ffi-sint ffi-uint32 6 ffi-pointer
    s\" SDL_CreateWindow\z" drop libsdl dlsym
    ffi-def sdl-create-window

\ void SDL_DestroyRenderer(SDL_Renderer * renderer)
ffi-pointer 1 ffi-void
    s\" SDL_DestroyRenderer\z" drop libsdl dlsym
    ffi-def sdl-destroy-renderer

\ void SDL_DestroyTexture(SDL_Texture * texture)
ffi-pointer 1 ffi-void
    s\" SDL_DestroyTexture\z" drop libsdl dlsym
    ffi-def sdl-destroy-texture

\ void SDL_DestroyWindow(SDL_Window * window)
ffi-pointer 1 ffi-void
    s\" SDL_DestroyWindow\z" drop libsdl dlsym
    ffi-def sdl-destroy-window

\ const Uint8* SDL_GetKeyboardState(int *numkeys)
ffi-pointer 1 ffi-pointer
    s\" SDL_GetKeyboardState\z" drop libsdl dlsym
    ffi-def sdl-get-keyboard-state

\ void SDL_FlushEvent(Uint32 type)
ffi-uint32 1 ffi-void
    s\" SDL_FlushEvent\z" drop libsdl dlsym
    ffi-def sdl-flush-event

\ Uint32 SDL_GetQueuedAudioSize(SDL_AudioDeviceID dev)
ffi-uint32 1 ffi-uint32
    s\" SDL_GetQueuedAudioSize\z" drop libsdl dlsym
    ffi-def sdl-get-queued-audio-size

\ SDL_bool SDL_HasEvent(Uint32 type)
ffi-uint32 1 ffi-sint
    s\" SDL_HasEvent\z" drop libsdl dlsym
    ffi-def sdl-has-event

\ int SDL_Init(Uint32 flags)
ffi-uint32 1 ffi-sint
    s\" SDL_Init\z" drop libsdl dlsym
    ffi-def sdl-init

\ SDL_AudioDeviceID SDL_OpenAudioDevice(const char *device, int iscapture,
\                                       const SDL_AudioSpec *desired,
\                                       SDL_AudioSpec *obtained,
\                                       int allowed_changes)
ffi-pointer ffi-sint ffi-pointer ffi-pointer ffi-sint 5 ffi-uint32
    s\" SDL_OpenAudioDevice\z" drop libsdl dlsym
    ffi-def sdl-open-audio-device

\ void SDL_PauseAudioDevice(SDL_AudioDeviceID dev, int pause_on)
ffi-uint32 ffi-sint 2 ffi-void
    s\" SDL_PauseAudioDevice\z" drop libsdl dlsym
    ffi-def sdl-pause-audio-device

\ void SDL_PumpEvents(void)
0 ffi-void
    s\" SDL_PumpEvents\z" drop libsdl dlsym
    ffi-def sdl-pump-events

\ int SDL_QueueAudio(SDL_AudioDeviceID dev, const void *data, Uint32 len)
ffi-uint32 ffi-pointer ffi-uint32 3 ffi-sint
    s\" SDL_QueueAudio\z" drop libsdl dlsym
    ffi-def sdl-queue-audio

\ void SDL_Quit(void)
0 ffi-void
    s\" SDL_Quit\z" drop libsdl dlsym
    ffi-def sdl-quit

\ int SDL_RenderCopy(SDL_Renderer * renderer, SDL_Texture * texture,
\                    const SDL_Rect * srcrect, const SDL_Rect * dstrect)
ffi-pointer ffi-pointer ffi-pointer ffi-pointer 4 ffi-sint
    s\" SDL_RenderCopy\z" drop libsdl dlsym
    ffi-def sdl-render-copy

\ void SDL_RenderPresent(SDL_Renderer * renderer)
ffi-pointer 1 ffi-void
    s\" SDL_RenderPresent\z" drop libsdl dlsym
    ffi-def sdl-render-present

\ int SDL_UpdateTexture(SDL_Texture * texture, const SDL_Rect * rect,
\                       const void *pixels, int pitch)
ffi-pointer ffi-pointer ffi-pointer ffi-sint 4 ffi-sint
    s\" SDL_UpdateTexture\z" drop libsdl dlsym
    ffi-def sdl-update-texture

$00008008 constant SDL-AUDIO-S8
$00000100 constant SDL-EVENT-QUIT
$00000200 constant SDL-EVENT-WINDOW
$00000010 constant SDL-INIT-AUDIO
$00000020 constant SDL-INIT-VIDEO
$00000002 constant SDL-RENDERER-ACCELERATED
$14110801 constant SDL-PIXELFORMAT-RGB332
$00000000 constant SDL-TEXTUREACCESS-STATIC
$00000004 constant SDL-WINDOW-SHOWN
$1fff0000 constant SDL-WINDOWPOS-UNDEFINED

create SDL-SCANCODE
    $1b c,  \ SDL_SCANCODE_X
    $1e c,  \ SDL_SCANCODE_1
    $1f c,  \ SDL_SCANCODE_2
    $20 c,  \ SDL_SCANCODE_3
    $14 c,  \ SDL_SCANCODE_Q
    $1a c,  \ SDL_SCANCODE_W
    $08 c,  \ SDL_SCANCODE_E
    $04 c,  \ SDL_SCANCODE_A
    $16 c,  \ SDL_SCANCODE_S
    $07 c,  \ SDL_SCANCODE_D
    $1d c,  \ SDL_SCANCODE_Z
    $06 c,  \ SDL_SCANCODE_C
    $21 c,  \ SDL_SCANCODE_4
    $15 c,  \ SDL_SCANCODE_R
    $09 c,  \ SDL_SCANCODE_F
    $19 c,  \ SDL_SCANCODE_V
        does> chars + c@ ;

( ################################################################### )
(                         Exception Words                             )
( ################################################################### )

-$1000 constant invalid-op
-$1001 constant cpu-stack-overflow
-$1002 constant cpu-stack-underflow
-$1003 constant unimplemented-sys-op
-$1004 constant data-abort
-$1005 constant bad-rom-file
-$1006 constant ui-error

( ################################################################### )
(                             UI Words                                )
( ################################################################### )

#22050 constant audio-dfreq

\ XXX: If this code is running, an e4 cell is guaranteed to be the same
\      size as a C function pointer. Therefore, we can safely use an e4
\      cell for the callback field.
\ struct SDL_AudioSpec {
\     int freq;
\     SDL_AudioFormat format;
\     Uint8 channels;
\     Uint8 silence;
\     Uint16 samples;
\     Uint16 padding;
\     Uint32 size;
\     SDL_AudioCallback callback;
\     void *userdata;
\ }
create audio-spec
                          audio-dfreq  ffi-sint    ffi, \ freq
    ffi-uint16  ffi-align SDL-AUDIO-S8 ffi-uint16  ffi, \ format
    ffi-uint8   ffi-align $1           ffi-uint8   ffi, \ channels
    ffi-uint8   ffi-align $0           ffi-uint8   ffi, \ silence
    ffi-uint16  ffi-align $400         ffi-uint16  ffi, \ samples
    ffi-uint16  ffi-align $0           ffi-uint16  ffi, \ padding
    ffi-uint32  ffi-align $0           ffi-uint32  ffi, \ size
                    align $0                          , \ callback
    ffi-pointer ffi-align $0           ffi-pointer ffi, \ userdata
                    align

create audio-buf $800 ffi-sint8 ffi-units allot align
here audio-buf - value audio-buf-sz

$0   value audio-buf-count
$0   value audio-dev
$0   value display-window
$0   value display-renderer
$0   value display-texture
$0   value display-df
$0   value ui-kbt
$49  value bg-color
$d1  value fg-color
#440 value buzzer-freq

variable keyboard-state does>
    @ swap SDL-SCANCODE chars + c@ ;

: audio-buf-init
    audio-dfreq buzzer-freq / dup   \ audio-period audio-period
    audio-buf-sz swap /             \ audio-period periods-per-buf
    over *                          \ audio-period count
    swap 2/ over                    \ count half-period count
    0 do                            \ count half-period
        dup i swap / 2 mod 0<> if -#100 else #100 then
        audio-buf i ffi-sint8 ffi-units + ffi-sint8 ffi!
    loop drop to audio-buf-count
    ;

: audio-queue
    audio-dfreq audio-dev sdl-get-queued-audio-size -
    dup 0> if
        audio-buf-count / 1+ 0 do
            audio-dev audio-buf audio-buf-count
            sdl-queue-audio 0<> if ui-error throw then
        loop
    else drop then
    ;

: buzzer-freq-set
    dup #20 > over #2049 < and if
        to buzzer-freq
    else drop then
    ;

: buzzer-start
    audio-dev $0 sdl-pause-audio-device
    ;

: buzzer-stop
    audio-dev $1 sdl-pause-audio-device
    ;

: display-dirty
    $1 to display-df
    ;

: display-refresh
    display-df if
        display-texture $0 rot $40 sdl-update-texture
            0<> if ui-error throw then
        display-renderer display-texture $0 $0 sdl-render-copy
            0<> if ui-error throw then
        display-renderer sdl-render-present
        $0 to display-df
    else drop then
    ;

: key-poll
    keyboard-state 0<>
    ;

: key-any
    -$1 ui-kbt 0= if
        $f $0 do i key-poll 0<> if
            drop i $7 to ui-kbt leave
        then loop
    then
    ;

: ui-alive
    sdl-pump-events
    SDL-EVENT-QUIT sdl-has-event 0<>
    ;

: ui-check-window
    sdl-pump-events
    SDL-EVENT-WINDOW sdl-has-event 0<> if
        SDL-EVENT-WINDOW sdl-flush-event
        display-dirty
    then
    ;

: ui-decrement-timers
    ui-kbt ?dup 0> if 1- to ui-kbt then
    ;

: ui-destroy
    audio-dev ?dup 0<> if sdl-close-audio-device then
    display-texture ?dup 0<> if sdl-destroy-texture then
    display-renderer ?dup 0<> if sdl-destroy-renderer then
    display-window ?dup 0<> if sdl-destroy-window then
    sdl-quit
    ;

: ui-init
    SDL-INIT-VIDEO SDL-INIT-AUDIO or sdl-init 0<> if ui-error throw then
    s\" chip8.fth\z" drop SDL-WINDOWPOS-UNDEFINED SDL-WINDOWPOS-UNDEFINED
        $400 $200 SDL-WINDOW-SHOWN sdl-create-window
    ?dup 0= if ui-error throw then dup to display-window
    -1 SDL-RENDERER-ACCELERATED sdl-create-renderer
    ?dup 0= if ui-error throw then dup to display-renderer
    SDL-PIXELFORMAT-RGB332 SDL-TEXTUREACCESS-STATIC $40 $20 sdl-create-texture
    ?dup 0= if ui-error throw then to display-texture
    0 sdl-get-keyboard-state ['] keyboard-state >body !
    0 0 audio-spec 0 0 sdl-open-audio-device
    ?dup 0= if ui-error throw then to audio-dev
    audio-buf-init
    ;

( ################################################################### )
(                            CPU Words                                )
( ################################################################### )

$200 value cpu-pc
$0   value cpu-sp
$0   value cpu-i
$0   value cpu-dt
$0   value cpu-st

time&date * * * + * value cpu-rand

create cpu-v     $10  chars allot does> swap chars + ;
create cpu-vram  $800 chars allot does> swap chars + ;
create cpu-stack $20  cells allot does> swap cells + ;
create cpu-ram
    ( font sprites )
    $f0 c, $90 c, $90 c, $90 c, $f0 c, \ 0
    $20 c, $60 c, $20 c, $20 c, $70 c, \ 1
    $f0 c, $10 c, $f0 c, $80 c, $f0 c, \ 2
    $f0 c, $10 c, $f0 c, $10 c, $f0 c, \ 3
    $90 c, $90 c, $f0 c, $10 c, $10 c, \ 4
    $f0 c, $80 c, $f0 c, $10 c, $f0 c, \ 5
    $f0 c, $80 c, $f0 c, $90 c, $f0 c, \ 6
    $f0 c, $10 c, $20 c, $40 c, $40 c, \ 7
    $f0 c, $90 c, $f0 c, $90 c, $f0 c, \ 8
    $f0 c, $90 c, $f0 c, $10 c, $f0 c, \ 9
    $f0 c, $90 c, $f0 c, $90 c, $90 c, \ A
    $e0 c, $90 c, $e0 c, $90 c, $e0 c, \ B
    $f0 c, $80 c, $80 c, $80 c, $f0 c, \ C
    $e0 c, $90 c, $90 c, $90 c, $e0 c, \ D
    $f0 c, $80 c, $f0 c, $80 c, $f0 c, \ E
    $f0 c, $80 c, $f0 c, $80 c, $80 c, \ F
    ( remaining memory )
                 $fb0 chars allot does> swap chars + ;

: cpu-pc-skip
    cpu-pc $2 + to cpu-pc
    ;

: cpu-stack-push
    cpu-sp $20 > if
        cpu-stack-overflow throw
    then
    >r cpu-sp dup cpu-stack
    r> swap !
    1+ to cpu-sp
    ;

: cpu-stack-pop
    cpu-sp 0> 0= if
        cpu-stack-underflow throw
    then
    cpu-sp 1- dup cpu-stack @
    swap to cpu-sp
    ;

: cpu-vram-clear
    $0 cpu-vram $800 chars bg-color fill
    ;

: cpu-rand-fetch
    ( Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" )
    cpu-rand dup #13 lshift xor
             dup #17 rshift xor
             dup  #5 lshift xor dup to cpu-rand
    ;

: split-op
    dup           $f and swap
    dup $4 rshift $f and swap
    dup $8 rshift $f and swap
        $c rshift $f and
    ;

: nnn
    $4 lshift or
    $4 lshift or
    ;

: kk
    $4 lshift or
    ;

: op-cls   cpu-vram-clear display-dirty ;
: op-ret   cpu-stack-pop to cpu-pc ;
: op-sys   unimplemented-sys-op throw ;
: op-jmp   nnn to cpu-pc ;
: op-call  cpu-pc cpu-stack-push op-jmp ;
: op-se    >r kk r>         cpu-v c@  = if cpu-pc-skip then ;
: op-sne   >r kk r>         cpu-v c@ <> if cpu-pc-skip then ;
: op-sre   cpu-v c@ swap    cpu-v c@  = if cpu-pc-skip then ;
: op-ld    >r kk r>         cpu-v                        c! ;
: op-add   >r kk r>         cpu-v dup c@ rot   + swap    c! ;
: op-mov   cpu-v swap       cpu-v     c@         swap    c! ;
: op-or    cpu-v dup c@ rot cpu-v     c@      or swap    c! ;
: op-and   cpu-v dup c@ rot cpu-v     c@     and swap    c! ;
: op-xor   cpu-v dup c@ rot cpu-v     c@     xor swap    c! ;
: op-addr  cpu-v dup c@ rot cpu-v     c@       + dup rot c!
                                   $ff > $1 and $f cpu-v c! ;
: op-subr  cpu-v dup c@ rot cpu-v     c@       - dup rot c!
                                   0< 0= $1 and $f cpu-v c! ;
: op-shr   cpu-v c@ dup             $1  rshift rot cpu-v c!
                                    $1      and $f cpu-v c! ;
: op-subnr cpu-v dup c@ rot cpu-v     c@  swap - dup rot c!
                              0< 0= $1      and $f cpu-v c! ;
: op-shl   cpu-v c@ dup             $1  lshift rot cpu-v c!
                         $7 rshift  $1      and $f cpu-v c! ;
: op-srne  cpu-v c@ swap cpu-v c@ <> if cpu-pc-skip then ;
: op-ldi   nnn to cpu-i ;
: op-jmpi  nnn $0 cpu-v c@ + to cpu-pc ;
: op-rand  cpu-v >r kk cpu-rand-fetch and r> c! ;
: op-draw
    $0 $f cpu-v c!
    cpu-v c@ swap cpu-v c@ rot          \ v[x] v[y] n
    $0 do
          dup i chars + $20 mod $40 *   \ v[x] v[y] vert
        cpu-i i chars + cpu-ram c@      \ v[x] v[y] vert sbyte
        $8 $0 do
            over $4 pick                \ v[x] v[y] vert sbyte vert v[x]
            i + $40 mod + cpu-vram      \ v[x] v[y] vert sbyte vram_offset
            over $80 i rshift and       \ v[x] v[y] vert sbyte vram_offset sbit
            0<> if                      \ v[x] v[y] vert sbyte vram_offset
                dup c@ fg-color = if
                    bg-color
                    $1 $f cpu-v c!
                else fg-color then swap c!
            else drop then
        loop 2drop
    loop 2drop
    display-dirty
    ;
: op-skp   cpu-v c@ key-poll 0<> if cpu-pc-skip then ;
: op-sknp  cpu-v c@ key-poll 0=  if cpu-pc-skip then ;
: op-movd  cpu-v cpu-dt swap c! ;
: op-key
    key-any dup 0> if swap cpu-v c!
    else 2drop cpu-pc $2 - to cpu-pc then ;
: op-ldd   cpu-v c@ to cpu-dt ;
: op-lds   cpu-v c@ dup to cpu-st 0> if buzzer-start then ;
: op-addi  cpu-v c@ cpu-i + to cpu-i ;
: op-ldspr cpu-v c@ $5    * to cpu-i ;
: op-bcd
    cpu-v c@  dup #100 / dup cpu-i      chars cpu-ram c!  \ h.t.o h
    #100 * -  dup #10  / dup cpu-i 1+   chars cpu-ram c!  \ 0.t.o t
    #10  * -                 cpu-i #2 + chars cpu-ram c! ;
: op-str
    dup chars cpu-i + $1000 > if data-abort throw then
    1+ chars $0   cpu-v   cpu-i cpu-ram rot move ;
: op-read
    dup chars cpu-i + $1000 > if data-abort throw then
    1+ chars cpu-i cpu-ram   $0 cpu-v   rot move ;

: interpret-op
    split-op case
        $0 of nnn dup case
            $e0 of drop op-cls endof
            $ee of drop op-ret endof
            op-sys
        endcase endof
        $1 of op-jmp  endof
        $2 of op-call endof
        $3 of op-se   endof
        $4 of op-sne  endof
        $5 of rot $0 =
            if op-sre
            else invalid-op throw
        then endof
        $6 of op-ld endof
        $7 of op-add endof
        $8 of rot case
            $0 of op-mov   endof
            $1 of op-or    endof
            $2 of op-and   endof
            $3 of op-xor   endof
            $4 of op-addr  endof
            $5 of op-subr  endof
            $6 of op-shr   endof
            $7 of op-subnr endof
            $e of op-shl   endof
            invalid-op throw
        endcase endof
        $9 of rot $0 =
            if op-srne
            else invalid-op throw
        then endof
        $a of op-ldi  endof
        $b of op-jmpi endof
        $c of op-rand endof
        $d of op-draw endof
        $e of rot rot kk case
            $9e of op-skp  endof
            $a1 of op-sknp endof
            invalid-op throw
        endcase endof
        $f of rot rot kk case
            $07 of op-movd  endof
            $0a of op-key   endof
            $15 of op-ldd   endof
            $18 of op-lds   endof
            $1e of op-addi  endof
            $29 of op-ldspr endof
            $33 of op-bcd   endof
            $55 of op-str   endof
            $65 of op-read  endof
            invalid-op throw
        endcase endof
        invalid-op throw
    endcase
    ;

: cpu-tick
    cpu-pc dup dup
         $1 chars + cpu-ram c@ swap
                    cpu-ram c@ $8 lshift or
    swap $2 chars + to cpu-pc
    interpret-op
    ;

: cpu-decrement-timers
    cpu-dt ?dup 0> if 1- to cpu-dt then
    cpu-st ?dup 0> if
        1- dup to cpu-st
        0= if buzzer-stop then
    then
    ;

: cpu-execute
    cpu-vram-clear
    $0 begin
        1+ dup $8 mod 0= if
            cpu-decrement-timers
            ui-decrement-timers
            audio-queue
            ui-check-window
            0 cpu-vram display-refresh
        then
        ui-alive 0<> if 0 exit then
        $2 ms ['] cpu-tick catch ?dup 0<>
    until
    cpu-st 0<> if buzzer-stop then
    swap drop dup case
        invalid-op           of ." Invalid op at " cpu-pc $2 - . cr endof
        cpu-stack-overflow   of ." CPU stack overflow" cr           endof
        cpu-stack-underflow  of ." CPU stack underflow" cr          endof
        unimplemented-sys-op of ." Unknown op at " cpu-pc $2 - . cr endof
        data-abort           of ." Data abort" cr                   endof
        throw
    endcase drop
    ;

: load-rom
    r/o open-file 0<> if bad-rom-file throw then
    dup >r $200 cpu-ram $e00 r>
    read-file 0<> if bad-rom-file throw then
    drop close-file drop
    ;

( ################################################################### )
(                           Main Program                              )
( ################################################################### )

: argv-toi
    0 s>d rot argv >number 2drop drop
    ;

: main
    $1 argv 2dup
    ." Attempting to load ROM: " type cr
    ['] load-rom catch dup case
        bad-rom-file of ." Bad ROM file." cr drop bye endof
        throw
    endcase
    ." Okay. Beginning execuption..." cr
    ['] ui-init catch dup case
        ui-error of
            ." Failed to initialize SDL. Terminating." cr drop
            ui-destroy
            bye
        endof
        throw
    endcase
    ['] cpu-execute catch
    ui-destroy
    dup case
        -#28 of ." Caught user interrupt. Terminating." drop cr endof
        throw
    endcase
    bye
    ;

." Welcome to chip8.fth, an e4 CHIP-8 emulator!" cr

argc $1 > [if]
    \ Allow custom background and foreground colors to be specified.
    base @ decimal
    argc $2 > [if] $2 argv-toi buzzer-freq-set [then]
    hex
    argc $3 > [if] $3 argv-toi to bg-color [then]
    argc $4 > [if] $4 argv-toi to fg-color [then]
    base !
    main
[else]
    ." No ROM file specified. Entering interactive mode." cr
[then]
