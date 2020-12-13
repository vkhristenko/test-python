#!/bin/python3
#
# running a simple search over CNN site
#

import requests
from bs4 import BeautifulSoup

def get_links(cnn_soup):
    #print(cnn_soup.prettify())
    links = []
    for i in cnn_soup.find("div", {"class": "zn__containers"}).find_all("a"):
        print(i)
    return links

def main():
    r = requests.get("https://edition.cnn.com/")
    cnn = BeautifulSoup(r.content, "html.parser")

    links = get_links(cnn)

if __name__ == "__main__":
    main()
