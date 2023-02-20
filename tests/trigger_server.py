import socket

def run_server():
    s = socket.socket()
    print("Socket successfully created")
    ok = "ok".encode('utf-8')

    port = 8881
    s.bind(('', port))
    print("socket binded to %s" % (port))
    s.listen(5)
    print("socket is listening")

    while True:
        try:
            c, addr = s.accept()
            print('Got connection from', addr)

            while True:
                data = c.recv(5012)
                if not data:
                    # Connection lost, break out of inner loop and reconnect
                    print("Connection lost, reconnecting...")
                    c.close()
                    break

                if data == b'trigger\r\n':
                    print(f"received: {data}")
                    print(f"sent: {ok}")
                    c.send(ok)
                if data == b'stop\r\n':
                    print(f"received: {data}")
                    print(f"sent: {ok}")
                    c.send(ok)
                else:
                    print(f"new data coming: {data}")


            
        except KeyboardInterrupt:
            print("Program stopped by user.")
            break
        except Exception as e:
            print("Error occurred:", e)

    s.close()

if __name__ == "__main__":
    run_server()
