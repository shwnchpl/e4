extern "C" {
    #include "e4.h"
}

namespace {
    char task_buffer[2048];
    struct e4__task *task;

    e4__usize serial_accept(void *user, char *buf, e4__usize *n)
    {
        *n = Serial.readBytesUntil('\r', buf, *n);
        return e4__E_OK;
    }

    e4__usize serial_key(void *user, char *buf)
    {
        register signed char c;

        while ((c = Serial.read() < 0)) ;
        *buf = c;

        return e4__E_OK;
    }

    e4__usize serial_ms(void *user, e4__usize ms)
    {
        delay(ms);
        return e4__E_OK;
    }

    e4__usize serial_type(void *user, const char *buf, e4__usize n)
    {
        Serial.write(buf, n);
        return e4__E_OK;
    }
}

void setup() {
    static const struct e4__io_func io_src = {
        NULL,
        serial_accept,
        serial_key,
        serial_type,
        NULL,
        serial_ms,
        NULL,
    };

    Serial.begin(9600);
    Serial.setTimeout(-1);
    while (!Serial) ;

    task = e4__task_create(task_buffer, sizeof(task_buffer));
    e4__task_io_init(task, &io_src);

    /* Add dictionary entries to allow for LED control. */
    e4__dict_entry(task, "led-pin", 7, 0, e4__execute_userval,
            (void *)digitalPinToBitMask(LED_BUILTIN));
    e4__dict_entry(task, "led-mode", 8, 0, e4__execute_userval,
            (void *)portModeRegister(digitalPinToPort(LED_BUILTIN)));
    e4__dict_entry(task, "led-out", 7, 0, e4__execute_userval,
            (void *)portOutputRegister(digitalPinToPort(LED_BUILTIN)));
}

void loop() {
    e4__evaluate_quit(task);
}

/* vim: set ft=cpp: */
