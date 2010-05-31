#!/usr/bin/make -f

build: springlobby.6 springsettings.6

%.6 : %.6.xml
	xsltproc /usr/share/xml/docbook/stylesheet/nwalsh/manpages/docbook.xsl $<

%.6.xml : %.6.txt
	asciidoc -d manpage -b docbook $<

clean:
	rm -f *.6