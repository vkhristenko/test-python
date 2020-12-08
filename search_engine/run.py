import requests
from bs4 import BeautifulSoup

r = requests.get("https://bola.kompas.com")
soup = BeautifulSoup(r.content, "html.parser")

links = []
for i in soup.find("div", {"class":"most__wrap"}).find_all("a"):
    i["href"] = i["href"] + "?page=all"
    links.append(i["href"])

print("------ Links ------")
print(links)
print("------ End of Links ------")

docs = []
for i in links:
    r = requests.get(i)
    soup = BeautifulSoup(r.content, "html.parser")
    sen = []
    for i in soup.find("div", {"class":"read__content"}).find_all("p"):
        sen.append(i.text)

    docs.append(" ".join(sen))

print("------ Docs -----")
print(docs)
print("------ End of Docs -----")

import re
import string

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

print("------ Cleaned Docs -----")
print(docs_clean)
print("------ End of Cleaned Docs -----")

from sklearn.feature_extraction.text import TfidfVectorizer
import pandas as pd
import numpy as np
# Instantiate a TfidfVectorizer object
vectorizer = TfidfVectorizer()
# It fits the data and transform it as a vector
X = vectorizer.fit_transform(docs_clean)
# Convert the X as transposed matrix
X = X.T.toarray()
# Create a DataFrame and set the vocabulary as the index
df = pd.DataFrame(X, index=vectorizer.get_feature_names())

print(df)

def get_similar_articles(q, df):
  print("query:", q)
  # Convert the query become a vector
  q = [q]
  q_vec = vectorizer.transform(q).toarray().reshape(df.shape[0],)
  sim = {}
  # Calculate the similarity
  for i in range(10):
    sim[i] = np.dot(df.loc[:, i].values, q_vec) / np.linalg.norm(df.loc[:, i]) * np.linalg.norm(q_vec)

  # Sort the values
  sim_sorted = sorted(sim.items(), key=lambda x: x[1], reverse=True)
  # Print the articles and their similarity values
  for k, v in sim_sorted:
    if v != 0.0:
      print("Nilai Similaritas:", v)
      print(docs[k])
      print()

q1 = "another day in"
get_similar_articles(q1, df)
