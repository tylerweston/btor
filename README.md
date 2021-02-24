**todo:** figure out why this is failing, should be using msvc instead of MSbuild?   
[![MSBuild](https://github.com/tylerweston/btor/actions/workflows/msbuild.yml/badge.svg)](https://github.com/tylerweston/btor/actions/workflows/msbuild.yml)  
**todo:** run unit tests
  
  
# btor
BitTorrent Client in C++ - Tyler Weston - 2021
  
What is implemented so far:  
- Bencoding parser
- Make properly formatted get request to server
- Get and parse list of peers from server

To do still:  
- better error handling
- implement more unit tests
- make connections with peers (multiple threads)
- send & receive handshakes
- exchange pieces
- verify pieces
- reassemble pieces
