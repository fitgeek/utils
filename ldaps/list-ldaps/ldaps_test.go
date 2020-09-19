package main

import (
	"reflect"
	"testing"
)

func Test_getConfig(t *testing.T) {
	type args struct {
		configFile string
	}
	tests := []struct {
		name string
		args args
		want map[string]string
	}{
		// TODO: Add test cases.
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			if got := getConfig(tt.args.configFile); !reflect.DeepEqual(got, tt.want) {
				t.Errorf("getConfig() = %v, want %v", got, tt.want)
			}
		})
	}
}

func Test_check(t *testing.T) {
	type args struct {
		msg string
		e   error
	}
	tests := []struct {
		name string
		args args
	}{
		// TODO: Add test cases.
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			check(tt.args.msg, tt.args.e)
		})
	}
}

func Test_credentials(t *testing.T) {
	tests := []struct {
		name  string
		want  string
		want1 string
	}{
		// TODO: Add test cases.
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			got, got1 := credentials()
			if got != tt.want {
				t.Errorf("credentials() got = %v, want %v", got, tt.want)
			}
			if got1 != tt.want1 {
				t.Errorf("credentials() got1 = %v, want %v", got1, tt.want1)
			}
		})
	}
}

func Test_main(t *testing.T) {
	tests := []struct {
		name string
	}{
		// TODO: Add test cases.
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			main()
		})
	}
}
