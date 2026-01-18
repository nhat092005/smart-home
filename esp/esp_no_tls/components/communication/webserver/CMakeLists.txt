idf_component_register(
    SRCS
    "webserver.c"
    INCLUDE_DIRS "include"
    REQUIRES
    esp_http_server
    json_helper
    wifi_manager
    EMBED_FILES
    "web/index.html"
    "web/style.css"
    "web/script.js"
)
