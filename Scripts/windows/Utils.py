import sys
import time
import urllib.request

def DownloadFile(url, filepath):
    print(f"Downloading {url}")
    req = urllib.request.Request(url, headers={'User-Agent': 'Mozilla/5.0'})

    with urllib.request.urlopen(req) as response, open(filepath, 'wb') as f:
        total = response.headers.get('content-length')

        if total is None:
            f.write(response.read())
            sys.stdout.write('\n')
            return

        downloaded = 0
        total = int(total)
        startTime = time.time()
        chunk_size = max(int(total / 1000), 1024 * 1024)

        while True:
            data = response.read(chunk_size)
            if not data:
                break
            downloaded += len(data)
            f.write(data)
            done = int(50 * downloaded / total)
            percentage = (downloaded / total) * 100
            elapsed = time.time() - startTime
            avgKBs = (downloaded / 1024) / elapsed if elapsed > 0 else 0
            speed = f'{avgKBs:.2f} KB/s' if avgKBs < 1024 else f'{avgKBs / 1024:.2f} MB/s'
            sys.stdout.write('\r[{}{}] {:.2f}% ({})     '.format(
                '█' * done, '.' * (50 - done), percentage, speed))
            sys.stdout.flush()
    sys.stdout.write('\n')

def YesOrNo(prompt="Continue?"):
    while True:
        reply = str(input(f'{prompt} [Y/N]: ')).lower().strip()
        if reply[:1] == 'y':
            return True
        if reply[:1] == 'n':
            return False