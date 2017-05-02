set(ICONS
    hicolor_apps_16x16_xneur.png
    hicolor_apps_22x22_xneur.png
    hicolor_apps_24x24_xneur.png
    hicolor_apps_32x32_xneur.png
    hicolor_apps_48x48_xneur.png
    hicolor_apps_scalable_xneur.svg
    )

foreach(iterator ${ICONS})
    string(REPLACE "_" ";" PARSED_LIST ${iterator})
    list(GET PARSED_LIST 0 THEME)
    list(GET PARSED_LIST 1 CONTEXT)
    list(GET PARSED_LIST 2 SIZE)
    list(GET PARSED_LIST 3 ICONFILE)
    install(FILES ${iterator} DESTINATION share/icons/${THEME}/${SIZE}/${CONTEXT} RENAME ${ICONFILE})
endforeach(iterator)
