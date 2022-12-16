#pragma once

#include <cstdlib>

namespace blobfish::runtime {

struct RuntimeConfig {
	/**
	 * Defaults to nproc
	 */
	size_t net_io_threads;

	/**
	 * Defaults to 1
	 */
	size_t file_io_threads;

	/**
	 * Defaults to nproc
	 */
	size_t compute_threads;

	/**
	 * Defaults to 64
	 */
	size_t max_coroutines;
};

}