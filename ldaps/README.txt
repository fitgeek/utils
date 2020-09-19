
ldaps.go development/build/test


Build go program
# go build ldaps.go

Execute command line
# ./ldaps configurationFile

Get gotests and install in $GOPATH/bin
The line below is the correct command string for getting and installing
# go get -u github.com/cweill/gotests/...

Create test files (ldaps_test.go)
# $GOPATH/bin/gotests -w -all ldaps.go

Test executable using the created tests
# go test ldaps

check-in to source control ldaps, ldaps_test.go
