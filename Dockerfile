FROM ubuntu:latest

FROM python:3.6

COPY xtract_c-code_main.py /
COPY testfile.c /
COPY requirements.txt /

RUN pip install -r requirements.txt

ENV CONTAINER_VERSION=1.0
