FROM ubuntu:latest

FROM python:3.6

COPY xtract_c_main.py /
COPY testfile.c /