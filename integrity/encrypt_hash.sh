#!/bin/bash
openssl enc -aes-256-cbc -salt -in /data/integrity/hash_store.json -out /data/integrity/encrypted_hash_store.enc -k "<your_password>"

