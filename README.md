The Open Middleware Agnostic Messaging API
================================================================================

OpenMAMA is a high performance vendor neutral lightweight wrapper that provides a common API interface
to different middleware and messaging solutions across a variety of platforms and languages.

OpenMAMDA is a framework that adds Market Data functionality, such as order book handling on top of MAMA.

The [project's homepage](https://openmama.org) provides a good overview of the project.

You can also pop into our gitter channel to have a chat if you just want quick answers:

[![Join the chat at https://gitter.im/OpenMAMA/OpenMAMA](https://badges.gitter.im/OpenMAMA/OpenMAMA.svg)](https://gitter.im/OpenMAMA/OpenMAMA?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Build Status
--------------------------------------------------------------------------------

We currently use travis for smoke testing of pull requests and our own Jenkins install for generating release artifacts.

| Branch          | Status      |
|-----------------|---------------|
| master (stable) |[![Build status](https://ci.appveyor.com/api/projects/status/4xo7i60r36vaeigi/branch/master?svg=true)](https://ci.appveyor.com/project/finos/openmama/branch/master)|
| next (unstable) |[![Build status](https://ci.appveyor.com/api/projects/status/4xo7i60r36vaeigi/branch/next?svg=true)](https://ci.appveyor.com/project/finos/openmama/branch/next)|

Supported Platforms
--------------------------------------------------------------------------------
Currently C, C++, C# and Java are all supported languages and Linux and Windows are supported platforms.

You can find more details on supported platforms [here](https://openmama.github.io/openmama_supported_platforms.html)

Docker
--------------------------------------------------------------------------------
For docker support for OpenMAMA, please see [our docker guide](docker/README.md).

Downloading the Software
--------------------------------------------------------------------------------
You can find the latest stable releases [on the our github releases page](https://github.com/OpenMAMA/OpenMAMA/releases)
or alternatively you can check out our
[cloudsmith repositories](https://cloudsmith.io/~openmama/repos/openmama/setup/)
which can provide stable or unstable builds depending on which repository you select.

Getting Started
--------------------------------------------------------------------------------
If you want to dive in, take a look at [our quick start guide](https://openmama.github.io/quickstart)

Documentation
--------------------------------------------------------------------------------
We host the latest OpenMAMA Technical documentation on [http://openmama.github.io](http://openmama.github.io)

Licensing
--------------------------------------------------------------------------------
This software is licensed under LGPL 2.1. Full terms are included in the `LICENSE.md` file. This software also
depends on several third party libraries, the licenses for which are listed in the `LICENSE-3RD-PARTY.txt` file.

Contributing
--------------------------------------------------------------------------------
Information on contributing on the project can be found [here](https://openmama.github.io/openmama_submission_process.html).

