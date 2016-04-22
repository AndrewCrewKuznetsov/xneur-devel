EXTRA_DIST = kdeneur_ru_RU.qm

trans_DATA =kdeneur_ru_RU.qm
transdir   = ${pkgdatadir}/i18n

kdeneur_ru_RU.qm:
	lupdate -verbose ../src/*.cpp ../src/*.h ../src/*.ui -ts kdeneur_ru_RU.ts
	lrelease kdeneur_ru_RU.ts

CLEANFILES = $(trans_DATA)
DISTCLEANFILES = $(trans_DATA)
MAINTAINERCLEANFILES = $(trans_DATA)

%.ts: %.qm
	lrelease *.qm

