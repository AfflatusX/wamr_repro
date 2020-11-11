WAMR Zephyr Repro
###########

Overview
********

This is a repro repository to demo issues related to WAMR + Zephyr combination. It uses Zephyr's Posix support so one does not need a physical board to reproduce the issue.

This has only been tested in Ubuntu - but any Linux distro should work.

Building and Running
********************

This repo only contains the application part - so one needs to set up zephyr environment first.

https://docs.zephyrproject.org/2.3.0/getting_started/index.html

**NOTE:** Make sure when you run ``west init ~/zephyrproject``, you specify the version: ``west init ~/zephyrproject --mr v2.3.0`` otherwise it would use the latest zephyr (which is 2.4 currently).

You should be able to run following command to have hello world app running:

.. code-block:: console

   west build -b native_posix samples/hello_world
   west build -t run


After zephyr is set up, simply download this repo and unzip it under ``~/zephyrproject/zephyr/samples/wamr`` and run

.. code-block:: console
   cd ~/zephyrproject/zephyr
   rm -rf build
   west build -b native_posix samples/wamr
   west build -t run

Sample Output
=============

.. code-block:: console

   ** Booting Zephyr OS build zephyr-v2.3.0  ***
   Hello World! native_posix
   [WASM][INFO] WASM engine thread created.
   [WASM][INFO] waiting for engine to be ready.
   App Manager started.
   [WASM][INFO] engine is now ready.
   [WASM][INFO] Dispatched install milan_wasm app request.
   timer ctx created. pre-alloc: 20
   Install WASM app success!
   WASM app 'milan_wasm' started
   hey hey yoyo 2
   
The Bug
=============

In ``app/assmebly/index.ts`` we have one more line in ``on_init()`` uses setTimeout() to print another message, but that never got printed.
