import whoosh
from whoosh.index import create_in
from whoosh.fields import *
from bs4 import BeautifulSoup
import string
import sys,urllib2,urllib
from whoosh.qparser import QueryParser



#timeout = 3
#socket.setdefaulttimeout(timeout)


schema = Schema(title=TEXT(stored=True), path=ID(stored=True), content=TEXT)
ix = create_in("test", schema)
writer = ix.writer()

count = 0

with open('links.txt') as fp:
    for line in fp:
	count = count + 1
	url = line
	if ".pdf" not in url:
		print line
		try:
			f = urllib.urlopen(url)
			content1 = f.read()
			soup = BeautifulSoup(content1)
			title = soup.get_text() 
			writer.add_document(title=unicode(url,"utf-8"), path=unicode("/"+url,"utf-8"),content=unicode(title))
		except Exception as e:
			print "Caught exception e"
	print count
writer.commit()




