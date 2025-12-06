#!/bin/sh
echo "=== Testing prodcons-sem 1 1 ==="
prodcons-sem 1 1
echo ""
echo "=== Testing prodcons-sem 2 3 ==="
prodcons-sem 2 3
echo ""
echo "=== Testing prodcons-sem 5 2 ==="
prodcons-sem 5 2
echo ""
echo "=== Tests complete ==="
