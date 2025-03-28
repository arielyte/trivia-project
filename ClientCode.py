import socket
import json

def send_message(server_ip, port, message_code, json_message):
    try:
        # Convert JSON message to bytes
        json_bytes = json_message.encode('utf-8')
        # Calculate message size
        message_size = len(json_bytes)
        # Construct message with code and size
        message = bytes([message_code]) + message_size.to_bytes(4, byteorder='big') + json_bytes

        # Create a socket object
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            # Connect to the server
            s.connect((server_ip, port))
            # Send the message
            s.sendall(message)
            print("Message sent successfully!")

            # Receive response from the server
            response = s.recv(1024)
            print("Response received from server:", response.decode('utf-8'))

            # Convert JSON message to bytes
            s.sendall(bytes([108]))
            response = s.recv(1024)
            print("Response received from server:", response.decode('utf-8'))

            """# Convert JSON message to bytes
            json_bytes = json.dumps({"RoomName": "hi", "MaxUsers": 3, "QuestionCount": 4, "AnswerTimeout": 30}).encode('utf-8')
            #json_bytes = json.dumps({"RoomID": 0}).encode('utf-8')
            # Calculate message size
            message_size = len(json_bytes)
            message = bytes([105]) + message_size.to_bytes(4, byteorder='big') + json_bytes
            s.sendall(message)
            response = s.recv(1024)
            print("Response received from server:", response.decode('utf-8'))


            # Convert JSON message to bytes
            s.sendall(bytes([112]))
            response = s.recv(1024)
            print("Response received from server:", response.decode('utf-8'))



            # Convert JSON message to bytes
            s.sendall(bytes([116]))
            response = s.recv(1024)
            print("Response received from server:", response.decode('utf-8'))

            # Convert JSON message to bytes
            s.sendall(bytes([116]))
            response = s.recv(1024)
            print("Response received from server:", response.decode('utf-8'))

            # Convert JSON message to bytes
            s.sendall(bytes([116]))
            response = s.recv(1024)
            print("Response received from server:", response.decode('utf-8'))

            # Convert JSON message to bytes
            s.sendall(bytes([116]))
            response = s.recv(1024)
            print("Response received from server:", response.decode('utf-8'))

            # Convert JSON message to bytes
            s.sendall(bytes([116]))
            response = s.recv(1024)
            print("Response received from server:", response.decode('utf-8'))

            # Convert JSON message to bytes
            json_bytes = json.dumps({"AnswerID": 2, "AnswerTimeSeconds": 10}).encode('utf-8')
            #json_bytes = json.dumps({"RoomID": 0}).encode('utf-8')
            # Calculate message size
            message_size = len(json_bytes)
            message = bytes([114]) + message_size.to_bytes(4, byteorder='big') + json_bytes
            s.sendall(message)
            response = s.recv(1024)
            print("Response received from server:", response.decode('utf-8'))
            # Convert JSON message to bytes
            json_bytes = json.dumps({"AnswerID": 1, "AnswerTimeSeconds": 4}).encode('utf-8')
            #json_bytes = json.dumps({"RoomID": 0}).encode('utf-8')
            # Calculate message size
            message_size = len(json_bytes)
            message = bytes([114]) + message_size.to_bytes(4, byteorder='big') + json_bytes
            s.sendall(message)
            response = s.recv(1024)
            print("Response received from server:", response.decode('utf-8'))
            # Convert JSON message to bytes
            json_bytes = json.dumps({"AnswerID": 1, "AnswerTimeSeconds": 29}).encode('utf-8')
            #json_bytes = json.dumps({"RoomID": 0}).encode('utf-8')
            # Calculate message size
            message_size = len(json_bytes)
            message = bytes([114]) + message_size.to_bytes(4, byteorder='big') + json_bytes
            s.sendall(message)
            response = s.recv(1024)
            print("Response received from server:", response.decode('utf-8'))
            # Convert JSON message to bytes
            json_bytes = json.dumps({"AnswerID": 1, "AnswerTimeSeconds": 15}).encode('utf-8')
            #json_bytes = json.dumps({"RoomID": 0}).encode('utf-8')
            # Calculate message size
            message_size = len(json_bytes)
            message = bytes([114]) + message_size.to_bytes(4, byteorder='big') + json_bytes
            s.sendall(message)
            response = s.recv(1024)
            print("Response received from server:", response.decode('utf-8'))


            # Convert JSON message to bytes
            s.sendall(bytes([117]))
            response = s.recv(1024)
            print("Response received from server:", response.decode('utf-8'))"""







    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    # Replace 'server_ip' with the actual IP address of the server
    server_ip = 'localhost'  # Change this to the IP address of your server
    port = 8200  # Port to connect to

    # JSON messages
    signup_json = {"username": "yonatan", "password": "1234"}

    send_message(server_ip, port, 101, json.dumps(signup_json))

