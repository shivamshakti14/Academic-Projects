import whoosh
import os, os.path
from whoosh import index
from whoosh.index import create_in
from whoosh.fields import *
from bs4 import BeautifulSoup
import string
import sys,urllib2,urllib
from whoosh.qparser import QueryParser
ix = index.open_dir("test")

with ix.searcher() as searcher:
	query = QueryParser("content", ix.schema).parse("hss")
	results = searcher.search(query)
	for i in results:
		print i['path']
