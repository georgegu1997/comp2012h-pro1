# HKUST 2016-2017 Fall COMP 2012H Project 1
# Author: GU Qiao
# Email: qgu@connect.ust.hk
# All rights reserved

# In this project, instead of (char buffer), I use the (struct package) as the form of the information transmitted.
# This made it very simple to identify the type of the information sent, and compare the prices in the request and in the server.

# Actually I found that the real time of transmission is so short that it is very difficult for a buy request to FAIL.
# So I added two sleep(1) after the generation of the packages and before write() function. These are the simply simulation of the delay of the network.
# After that, I found that the client can only send one request in one second, however, which is identical as the sample programs do.
# The failure of buy request now will appear at the last second before price changes, and may appear on other requests depending on the real delay of the network
