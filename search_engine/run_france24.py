#!/bin/python3
#
# running a simple search over https://www.france24.com site
# install requirements with
# python3 -m pip install bs4 requests sklearn pandas numpy
#

import requests
from bs4 import BeautifulSoup
import re, string
from sklearn.feature_extraction.text import TfidfVectorizer
import pandas as pd
import numpy as np
import sys
import json

def get_links(url):
    """
    Get all the references from the provided url
    """
    headers = {'User-agent': 'Mozilla/5.0'}
    r = requests.get(url, headers=headers)
    soup = BeautifulSoup(r.content, "html.parser")
    links = []
    divWithArticles = soup.find("div", {"class": "t-content t-content--page-builder"})
    for a in divWithArticles.find_all("a"):
        links.append(url + a["href"])
        
    return links

def get_docs(links):
    """
    Given all the links, extract the text/article documents
    """
    headers = {'User-agent': 'Mozilla/5.0'}
    docs = []
    counter = 0
    for l in links:
        print("processing link %d" % counter)
        r = requests.get(l, headers=headers)
        soup = BeautifulSoup(r.content, "html.parser")
        p = soup.find("p", {"class" : "t-content__chapo"})
        if p is None: continue
        #print(p.text)
        mainDiv = soup.find("div", {"class" : "t-content__body u-clearfix"})
        sens = [p.text]
        for pp in mainDiv.find_all("p"):
            sens.append(pp.text)
        docs.append(" ".join(sens))
        counter += 1
        
    return docs

def clean_docs(docs):
    """
    Clean up all the documents in order to simplify search thru them
    """
    docs_clean = []
    for d in docs:
        # Remove Unicode
        document_test = re.sub(r'[^\x00-\x7F]+', ' ', d)
        # Remove Mentions
        document_test = re.sub(r'@\w+', '', document_test)
        # Lowercase the document
        document_test = document_test.lower()
        # Remove punctuations
        document_test = re.sub(r'[%s]' % re.escape(string.punctuation), ' ', document_test)
        # Lowercase the numbers
        document_test = re.sub(r'[0-9]', '', document_test)
        # Remove the doubled space
        document_test = re.sub(r'\s{2,}', ' ', document_test)
        docs_clean.append(document_test)

    return docs_clean

def create_tfidf(docs):
    """
    Use TF-IDF as metric for identifying texts that are "similar" to the query
    """
    # Instantiate a TfidfVectorizer object
    vectorizer = TfidfVectorizer()
    # It fits the data and transform it as a vector
    X = vectorizer.fit_transform(docs)
    # Convert the X as transposed matrix
    X = X.T.toarray()
    # Create a DataFrame and set the vocabulary as the index
    df = pd.DataFrame(X, index=vectorizer.get_feature_names())
    return (df, vectorizer)

def run_query(query, df, vectorizer):
    """
    Run the query string against all the searched documents
    """
    print("query string:", query)

    # transform the query string into proper input form
    q = [query]
    q_vec = vectorizer.transform(q).toarray().reshape(df.shape[0],)

    # Calculate the similarity
    sim = {}
    for i in range(df.shape[1]):
        sim[i] = np.dot(df.loc[:, i].values, q_vec) / np.linalg.norm(df.loc[:, i]) * np.linalg.norm(q_vec)

    # Sort the values
    sim_sorted = sorted(sim.items(), key=lambda x: x[1], reverse=True)
    return sim_sorted

def print_usage():
    print("run with: python3 run_france24.py [--use-cache]")

def main():
    useCache = False
    if len(sys.argv)>1:
        if sys.argv[1] == "--help":
            print_usage()
            sys.exit(0)
        useCache = sys.argv[1] == "--use-cache"
    cacheFileName = "cache_france24.json"
    if useCache:
        with open(cacheFileName) as f:
            docs = json.load(f)
    else:
        url = "https://www.france24.com"

        # get all the links from "france24.com"
        links = get_links(url)
        print("--- we have {}".format(len(links)) + " links")

        # extract the documents
        docs = get_docs(links)
        #for d in docs[:1]:
        #    print("----------")
        #    print(d)
        #    print("----------")

        # clean it
        docs = clean_docs(docs)
        
        with open(cacheFileName, "w") as f:
            f.write(json.dumps(docs))            

    tmps = """
    print("we have {} documents".format(len(docs)))
    for d in docs:
        print("----------")
        print(d)
        print("----------")
    """
    # run the query 
    df, vectorizer = create_tfidf(docs)
    results = run_query(query, df, vectorizer)
    # show top 5 matches/results
    for k,v in results[:5]:
        if v>0:
            print("Similarity Value: ", v)
            print("---- Article -----")
            print(docs[k])
            print("------------------")

query = "students missing"

if __name__ == "__main__":
    main()
