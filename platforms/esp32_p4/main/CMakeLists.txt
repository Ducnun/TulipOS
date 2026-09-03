idf_component_register(
    SRCS "main.cpp" "esp32p4_host.cpp"
    INCLUDE_DIRS "."
    PRIV_REQUIRES core hardware_drivers
)