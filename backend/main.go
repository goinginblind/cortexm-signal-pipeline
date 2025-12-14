package main

import (
	"bufio"
	"encoding/binary"
	"fmt"
	"io"
	"os"
	"time"
)

func main() {
	filePath := "uart_output.log"
	var file *os.File
	for {
		f, err := os.Open(filePath)
		if err == nil {
			file = f
			break
		}
		fmt.Printf("Waiting for QEMU to create %s...\n", filePath)
		time.Sleep(100 * time.Millisecond)
	}
	defer file.Close()
	fmt.Printf("Connected to %s!\n", filePath)

	reader := bufio.NewReader(file)

	for {
		var magic uint16
		var count uint16
		var seq uint32

		if err := binary.Read(reader, binary.LittleEndian, &magic); err != nil {
			if err == io.EOF {
				fmt.Println("End of file, waiting for more data...")
				time.Sleep(100 * time.Millisecond)
				continue
			}
			fmt.Println("Error reading magic:", err)
			break
		}
		if err := binary.Read(reader, binary.LittleEndian, &count); err != nil {
			fmt.Println("Error reading count:", err)
			break
		}
		if err := binary.Read(reader, binary.LittleEndian, &seq); err != nil {
			fmt.Println("Error reading seq:", err)
			break
		}

		data := make([]uint16, count)
		if err := binary.Read(reader, binary.LittleEndian, &data); err != nil {
			fmt.Println("Error reading payload:", err)
			break
		}

		fmt.Printf("Frame %d: magic=0x%X count=%d first sample=%d\n",
			seq, magic, count, data[0])
	}
}
