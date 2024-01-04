# This script is only for testing purposes.

# It is generating random requests using curl command and sends them to the webserver,
# it catches the response and prints it to the results.txt file.

# This is only for testing webserver responses to be correct.

import random, subprocess, sys

host = sys.argv[1]
port = sys.argv[2]
methods = ["GET", "POST", "HEAD", "WRONG"]
paths = ["", "pages", "newpages/index.php", "thisShouldNotExist", "newpages", "index.php"]
versions = ["-1", "-0"] # first for HTTP/1.1 second for HTTP/1.0 (only the first is implemented)
post_attributes = '-d \"firstName=Gustav&lastName=Ferguson\"' # post attributes in curl command

request_number = 100

file = open("results.txt", "w")
file.write("")
file.close()

for i in range(request_number):
    file = open("results.txt", "a")
    method = random.choice(methods)
    
    if method == "POST":
        path = random.choice([random.choice(paths), "newpages/index.php"])
    else:
        path = random.choice(paths)
    version = random.choice(versions) if i % 10 == 0 else "-1"
    
    command = f"curl -i -X {method} {post_attributes if method == 'POST' else ''} {version} http://{host}:{port}/{path}"
    # file.write(command + '\n')
    print(command)
    result = subprocess.run(command, shell=True, capture_output=True, text=True)
    file.write(f"Command number: {i}\n")
    file.write(f"Command: {command}\n")
    file.write("--------------- CONTENT ---------------\n\n")
    file.write(result.stdout)
    file.write("----------------- END -----------------\n\n")
    print(f"Command {i} result: {result.returncode}")

    file.close()

