

from scrapy.selector import HtmlXPathSelector
from scrapy.spider import BaseSpider
from scrapy.http import Request

DOMAIN = 'intranet.iitg.ernet.in/'
URL = 'http://intranet.iitg.ernet.in/'
lis=[]
index_element = 0 

class MySpider(BaseSpider):
    name = 'crawler'
    #allowed_domains = [DOMAIN]
    start_urls = [
        URL
    ]

    def parse(self, response):
        hxs = HtmlXPathSelector(response)
        for url in hxs.select('//a/@href').extract():
            if not ( url.startswith('http://') or url.startswith('https://') ):
                url= URL + url
            if 'iitg' in url:
                if url not in lis:
                    lis.append(url) 
                    fo=open('finallink.txt','a')
                    fo.write(url + "\n")
                    fo.close()
                    yield Request(url, callback=self.parse)

