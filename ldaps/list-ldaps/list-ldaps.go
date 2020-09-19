package main

import (
	"bufio"
	"crypto/tls"
	"fmt"
	"golang.org/x/crypto/ssh/terminal"
	"gopkg.in/ldap.v3"
	"io/ioutil"
	"log"
	"os"
	"regexp"
	"strings"
	"syscall"
	"math"
)

func getConfig(configFile string) map[string]string {
	kv := make(map[string]string)
	dat, err := ioutil.ReadFile(configFile)
	check("Read Config File", err)
	rows := strings.Split(string(dat), "\n")
	for _, s := range rows { // index row
		// split on # and remove comment
		comment, _ := regexp.Match(`#`, []byte(s))
		if comment {
			s = strings.Split(s, "#")[0]
		}
		// skip blank lines
		if s == "" {
			continue
		}
		equal, _ := regexp.Match(`=`, []byte(s))
		if equal {
			keyval := strings.SplitN(s, "=", 2)
			kv[strings.ToLower(strings.TrimSpace(string(keyval[0])))] = strings.TrimSpace(string(keyval[1]))
		}
	}
	return kv
}

func check(msg string, e error) {
	if e != nil {
		fmt.Println(msg)
		log.Fatal(e)
		os.Exit(1)
	}
}

func credentials() (string, string) {
	reader := bufio.NewReader(os.Stdin)

	fmt.Print("Enter Username: ")
	username, _ := reader.ReadString('\n')

	fmt.Print("Enter Password: ")
	bytePassword, err := terminal.ReadPassword(int(syscall.Stdin))
	check("Read Password", err)
	password := string(bytePassword)

	fmt.Println("")
	return strings.TrimSpace(username), password
}

func main() {
	if len(os.Args) != 2 {
		fmt.Printf("Usage: %s /path/to/config-file", os.Args[0])
		fmt.Printf(`

For automation, the username and password to be authenticated can be passed
using the environment variables:
      LDAPUSER LDAPPASS

      ------Sample LDAPS Client Configuration File-----
# client ldap configuration file
server = ldap-sd.td.teradata.com
port = 389
service account = cn=rh122058,ou=application accounts,dc=td,dc=teradata,dc=com
service password = secret-password

`)
		os.Exit(1)
	}
	server := ""
	port := ""
	bindusername := ""
	bindpassword := ""
	kv := getConfig(os.Args[1])

	for key, val := range kv {
		if key == "server" {
			server = val
		}
		if key == "port" {
			port = val
		}
		if key == "service account" {
			bindusername = val
		}
		if key == "service password" {
			bindpassword = val
		}
		// fmt.Printf("key[%s] value[%s]\n", key, val)
	}

	username := os.Getenv("LDAPUSER")
	password := os.Getenv("LDAPPASS")
	if username == "" || password == "" {
		username, password = credentials()
	}
	// fmt.Printf("Username: %s, Password: %s\n", username, password)

	l, err := ldap.Dial("tcp", fmt.Sprintf("%s:%s", server, port))
	if err != nil {
		fmt.Println("Connection Error")
		log.Fatal(err)
	}
	defer l.Close()

	// Reconnect with TLS
	// For ldap, a client certificate is not required and is redundant as the
	// Transport Layer Security is handled via the server. 
	err = l.StartTLS(&tls.Config{InsecureSkipVerify: true})
	if err != nil {
		fmt.Println("TLS connection error")
		log.Fatal(err)
	}

	// First bind with a read only user
	err = l.Bind(bindusername, bindpassword)
	if err != nil {
		fmt.Println("Service Account Login Error")
		log.Fatal(err)
	}

	// Search for the given username
	searchRequest := ldap.NewSearchRequest(
		"dc=td,dc=teradata,dc=com",
		ldap.ScopeWholeSubtree, ldap.NeverDerefAliases, math.MaxInt64, 0, false,
		"(&(samaccountname=*))",
		[]string{"samaccountname","givenName", "sn", "displayName", "title", "cn"},
		nil,
	)

	// sr, err := l.Search(searchRequest)
	sr, err := l.SearchWithPaging(searchRequest, 100)
	if err != nil {
		log.Fatal(err)
	}

	for _, entry := range sr.Entries {
                if len(entry.GetAttributeValue("displayName")) > 0 && len(entry.GetAttributeValue("title")) > 0 {
			fmt.Printf("%s: %v\n", entry.GetAttributeValue("displayName"), entry.GetAttributeValue("cn"))
		}
	}
}
