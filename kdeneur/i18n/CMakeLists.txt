set (TR_FILES # ts files should be with full path, as TR_FILES is used in another location
	${PROJECT_SOURCE_DIR}/i18n/kdeneur_ru_RU.ts
)

set (TR_FILES ${TR_FILES} PARENT_SCOPE) # exporting to use in other CMakeLists.txt

#   string(ASCII 27 Esc)
#   message ("${Esc}[31m наш tr_files в i18n равен ${TR_FILES} ${Esc}[m")
