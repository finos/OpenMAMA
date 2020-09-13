#!/bin/bash

ansible-playbook -b --connection=local --inventory 127.0.0.1, demo-provision.yml
