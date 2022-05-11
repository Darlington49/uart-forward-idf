idf_component_register(SRCS "main.c"  "serial.c" "tcp.c" "mqtt.c" "sdcard" "file_server.c"
                    INCLUDE_DIRS "."
                    EMBED_FILES "favicon.ico" "upload_script.html")
