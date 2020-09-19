package main

import (
    "os"
    "fmt"
    "log"
    "bufio"
    "regexp"
    "syscall"
    "strings"
    "io/ioutil"
    "crypto/tls"
    "gopkg.in/ldap.v3"
    "golang.org/x/crypto/ssh/terminal"
)

var kv map[string]string
func init() {
    kv = make(map[string]string)
}

func getConfig(config_file string) {
    dat, err := ioutil.ReadFile(config_file)
    check(err)
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
}

func check(e error) {
    if e != nil {
        panic(e)
    }
}

func credentials() (string, string) {
    reader := bufio.NewReader(os.Stdin)

    fmt.Print("Enter Username: ")
    username, _ := reader.ReadString('\n')

    fmt.Print("Enter Password: ")
    bytePassword, err := terminal.ReadPassword(int(syscall.Stdin))
    check(err)
    password := string(bytePassword)

    fmt.Println("")
    return strings.TrimSpace(username), strings.TrimSpace(password)
}

func main() {
    if len(os.Args) != 2 {
	fmt.Printf("Usage: %s /path/to/config-file", os.Args[0])
        fmt.Printf(`
      ------Sample-----
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
    getConfig(os.Args[1])

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
        log.Fatal(err)
    }
    defer l.Close()

    // Reconnect with TLS
    err = l.StartTLS(&tls.Config{InsecureSkipVerify: true})
    if err != nil {
        log.Fatal(err)
    }
    
    // First bind with a read only user
    err = l.Bind(bindusername, bindpassword)
    if err != nil {
        log.Fatal(err)
    }
    
    // Search for the given username
    searchRequest := ldap.NewSearchRequest(
        "dc=td,dc=teradata,dc=com",
        ldap.ScopeWholeSubtree, ldap.NeverDerefAliases, 0, 0, false,
        fmt.Sprintf("(&(objectClass=organizationalPerson)(samaccountname=%s))", username),
        []string{"dn"},
        nil,
    )
    
    sr, err := l.Search(searchRequest)
    if err != nil {
        log.Fatal(err)
    }
    
    if len(sr.Entries) != 1 {
        log.Fatal("User does not exist or too many entries returned")
    }
    
    userdn := sr.Entries[0].DN
    
    // Bind as the user to verify their password
    err = l.Bind(userdn, password)
    if err != nil {
        log.Fatal(err)
    }
    
    // Rebind as the read only user for any further queries
    err = l.Bind(bindusername, bindpassword)
    if err != nil {
        log.Fatal(err)
    }
}
