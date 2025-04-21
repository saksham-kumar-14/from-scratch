package main

import (
	"bufio"
	"errors"
	"fmt"
	"os"
	"os/exec"
	"strings"
)

var noPathErr = errors.New("Path required")

func execInput(input string) error {
	input = strings.TrimSuffix(input, "\n");
	// Splits the statement
	args := strings.Split(input, " ")

	// switch statements for special command cmd, exit .... 
	switch args[0] {
		case "cd":
			if len(args) < 2{
				return noPathErr
			}
			return os.Chdir(args[1])

		case "exit" :
			os.Exit(0)
		
	}


	cmd := exec.Command(args[0], args[1:]...)
	cmd.Stderr = os.Stderr
	cmd.Stdout = os.Stdout
	return cmd.Run()
}

func main(){
	reader := bufio.NewReader(os.Stdin)
	for{
		fmt.Print("❯ ")
		input, err := reader.ReadString('\n')
		if err != nil{
			fmt.Fprintln(os.Stderr, err)
		}

		if err = execInput(input); err != nil{
			fmt.Fprintln(os.Stderr, err)
		}

	}

}
