import { on_timer_callback, setInterval, setTimeout } from "./timer";

@external('env', 'puts')
declare function printf(a: ArrayBuffer): i32;

export function log(a: string): void {
  printf(String.UTF8.encode(a, true));
}


export function on_init(): void {
  log("hey hey yoyo 2");
  setTimeout(() => {
    log("from timer yo!");
  }, 2000);
}


export function on_abort(
  message: string | null,
  fileName: string | null,
  lineNumber: u32,
  columnNumber: u32
): void {
  const l_msg = message;
  const l_filename = fileName;

  if (l_msg === null || l_filename === null) {
    log('aborting but message/filename is null.');
    return;
  }

  log(
    'Aborting: ' +
    l_msg +
    ' | file: ' +
    l_filename +
    ' | ' +
    lineNumber.toString() +
    ', ' +
    columnNumber.toString()
  );
}

export function _on_timer_callback(on_timer_id: i32): void {
  log("callback hit");
  on_timer_callback(on_timer_id);
}
