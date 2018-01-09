set(UI_FILES
    config.ui
    about.ui
    list.ui
    abbr_add.ui
    app_add.ui
    rule_add.ui
    usr_action_add.ui
    action_add.ui
    choose_file.ui
)

install(FILES ${UI_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/gxneur/ui)
